-- init tarantool db wor tests
box.cfg {
    slab_alloc_arena = 1,
    pid_file = 'tarantool.pid',
    wal_mode = 'none',
    snap_dir = '.',
    work_dir = '.',
}

local bench = require('cbench')


-- settings
local iterations = 2
local count = 1000000

local version = box.info.version

function split(str, delim)
    if string.find(str, delim) == nil then
        return { str }
    end
    local result, pat, lastpos = {}, "(.-)" .. delim .. "()", nil
    for part, pos in string.gfind(str, pat) do
        table.insert(result, part)
        lastpos = pos
    end
    table.insert(result, string.sub(str, lastpos))
    return result
end

function urlencode(t)
    local result = '?'
    for key, val in pairs(t) do
        if result ~= '?' then
            result = result .. '&'
        end
        result = result .. key .. '=' .. val
    end
    return result
end

-- Workloads
local tests = { 'replaces', 'selects', 'selrepl', 'updates', 'deletes' }
local workloads = {
    { tests = tests, type = 'hash', parts = { 'num' } },
    { tests = tests, type = 'hash', parts = { 'str' } },
    { tests = tests, type = 'hash', parts = { 'num', 'num' } },
    { tests = tests, type = 'hash', parts = { 'num', 'str' } },
    { tests = tests, type = 'hash', parts = { 'str', 'num' } },
    { tests = tests, type = 'hash', parts = { 'str', 'str' } },
    { tests = tests, type = 'tree', parts = { 'num' } },
    { tests = tests, type = 'tree', parts = { 'str' } },
    { tests = tests, type = 'tree', parts = { 'num', 'num' } },
    { tests = tests, type = 'tree', parts = { 'num', 'str' } },
    { tests = tests, type = 'tree', parts = { 'str', 'num' } },
    { tests = tests, type = 'tree', parts = { 'str', 'str' } }
}

function export(name, bench_key, value)
    local chart_name = name:gsub(' ', '_'):gsub('+_', ''):lower()
    local result = {
        key = token, name = 'cb.' .. bench_key .. '.' .. chart_name,
        param = tostring(math.floor(value)), unit = 'rps',
        tab = 'cbench.' .. chart_name, v = version
    }
    print(urlencode(result))
end

function run()
    local benches = bench.run(workloads, count, iterations)
    for _, data in pairs(benches) do
        local name = data[1]
        local series = data[2]
        for _, pair in pairs(series) do
            export(name, pair[1], pair[2])
        end
    end
end
run()
os.exit()
