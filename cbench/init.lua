--- Tarantool C Bench - a simple tool to benchmark Tarantool internal API
-- @copyright 2013 Tarantool Authors
-- @usage See example.lua for usage examples

local ffi = require("ffi")
ffi.cdef([[
    /* Generated from bench.h */

    /* {{{{ Utils */

    double
    nowtime(void);

    /* }}} */

    /* {{{ Generators */

    struct keygen_params {
        uint32_t len;
    };

    typedef char *(*keygen_t)(char *, const struct keygen_params *);

    char *
    gen_unsigned(char *r, const struct keygen_params *params);

    char *
    gen_str(char *r, const struct keygen_params *params);

    char *
    gen_unsigned_unsigned(char *r, const struct keygen_params *params);

    char *
    gen_str_str(char *r, const struct keygen_params *params);

    char *
    gen_unsigned_str(char *r, const struct keygen_params *params);

    char *
    gen_str_unsigned(char *r, const struct keygen_params *params);

    /* }}} */

    /* {{{ Workloads */

    struct test_params {
        keygen_t keygen;
        struct keygen_params *keygen_params;
        uint32_t space_id;
        uint32_t count;
    };

    typedef double (test_t)(const struct test_params *);

    void
    test_keys(const struct test_params *params);

    void
    test_selects(const struct test_params *params);

    void
    test_replaces(const struct test_params *params);

    void
    test_selrepl(const struct test_params *params);

    void
    test_updates(const struct test_params *params);

    void
    test_deletes(const struct test_params *params);

    /* }}} */
]])

ffi.cdef([[
    void srand(unsigned int seed);
]])

local builtin = ffi.C
local libbench = ffi.load(package.searchpath('cbench.bench', package.cpath))

-- Return current timestamp with float part
local nowtime = function()
    return tonumber(libbench.nowtime())
end

--- Calculate RPS
local calc_rps = function(delta, count)
    return count / delta;
end

--- Find keygen by name
local get_keygen = function(index_parts)
    return libbench["gen_"..table.concat(index_parts, "_")]
end

--- Find test by name
local get_test = function(name)
    return libbench["test_"..name]
end

-- Run single test and measure time
local bench = function(test, tparams)
    builtin.srand(0)
    local start_time = nowtime()
    test(tparams)
    local end_time = nowtime()
    return (end_time - start_time)
end

--- Main API - run benchmarks with provided workloads
-- @param workloads (table) workloads description
-- @param count (uint) how many keys to benchmark
-- @param rep (uint) how many times to repeat each workload
local run = function(workloads, count, rep, engine)
    if rep == nil then rep = 1 end
    -- Check arguments
    for _, wl in ipairs(workloads) do
        -- Check that generator exists
        get_keygen(wl.parts)
        -- Check tests
        for i, name in ipairs(wl.tests) do
            get_test(name)
        end
    end

    local result = {}
    local tparams = ffi.new('struct test_params')
    local kparams = ffi.new('struct keygen_params')

    for space_id, wl in ipairs(workloads) do
        space_id = space_id + box.schema.SYSTEM_ID_MAX
        kparams.len = 32
        tparams.keygen = get_keygen(wl.parts)
        tparams.keygen_params = kparams
        tparams.space_id = space_id
        tparams.count = count

        -- Create required spaces using box.schema API
        local space_name = 'space'..tostring(space_id)
        local space = box.schema.create_space(space_name, { engine = engine, id = space_id })
        local parts = {}
        for fno, ftype in ipairs(wl.parts) do
            table.insert(parts, fno)
            table.insert(parts, ftype)
        end
        space:create_index('primary', { type = wl.type, parts = parts })

        local rt = {}
        local rk = 0
        for wid, wname in ipairs(wl.tests) do
            rt[wid] = 0.0
        end
        for r=1,rep,1 do
            for wid, wname in ipairs(wl.tests) do
                -- Run required benchmarks
                rt[wid] = rt[wid] + bench(get_test(wname), tparams) / rep
            end
            -- Run keygen benchmark
            rk = rk + bench(get_test('keys'), tparams) / rep
        end

        local index_description = table.concat(wl.parts, " + ")

        local res = {}
        local wldescription = string.upper(wl.type) .. " " .. index_description
        print(wldescription)
        print('----------------------------------')
        for wid, wname in ipairs(wl.tests) do
            local rps = calc_rps(rt[wid] - rk, count)
            print(string.format("%-10s: %-11.2f rps", wname, rps))
            table.insert(res, {wname, rps})
        end
        table.insert(result, {wldescription, res})
        print('----------------------------------')
    end
    return result
end

-- Export Lua module
return {
    run = run
}
