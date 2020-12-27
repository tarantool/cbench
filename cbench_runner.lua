--- init tarantool db wor tests



if #arg < 1 then
    print('Please specify engine [memtx] or [vinyl]')
    os.exit(1)
end

local engine = arg[1]

if engine == 'vinyl' then
    if #arg < 2 then
        print('Please specify wal_mode [write] or [fsync]')
        os.exit(1)
    end
    wal_mode = arg[2]
    count = tonumber(arg[3])
elseif engine == 'memtx' then
    wal_mode = 'none'
    count = tonumber(arg[2])
end

box.cfg {
    slab_alloc_arena = 1,
    pid_file = 'tarantool.pid',
    wal_mode = wal_mode,
    snap_dir = '.',
    work_dir = '.',
}

local bench = require('cbench')


-- settings
local iterations = 2
if count == nil then
    count = 1000000
end

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
if engine == 'vinyl' then
    workloads = {
        { tests = tests, type = 'tree', parts = { 'num' } },
        { tests = tests, type = 'tree', parts = { 'str' } },
        { tests = tests, type = 'tree', parts = { 'num', 'num' } },
        { tests = tests, type = 'tree', parts = { 'num', 'str' } },
        { tests = tests, type = 'tree', parts = { 'str', 'num' } },
        { tests = tests, type = 'tree', parts = { 'str', 'str' } }
    }
end
if engine == 'memtx' then
    workloads = {
        { tests = tests, type = 'tree', parts = { 'num' } },
        { tests = tests, type = 'tree', parts = { 'str' } },
        { tests = tests, type = 'tree', parts = { 'num', 'num' } },
        { tests = tests, type = 'tree', parts = { 'num', 'str' } },
        { tests = tests, type = 'tree', parts = { 'str', 'num' } },
        { tests = tests, type = 'tree', parts = { 'str', 'str' } },
        { tests = tests, type = 'hash', parts = { 'num' } },
        { tests = tests, type = 'hash', parts = { 'str' } },
        { tests = tests, type = 'hash', parts = { 'num', 'num' } },
        { tests = tests, type = 'hash', parts = { 'num', 'str' } },
        { tests = tests, type = 'hash', parts = { 'str', 'num' } },
        { tests = tests, type = 'hash', parts = { 'str', 'str' } },
    }
end

function export(name, bench_key, value)
    local chart_name = name:gsub(' ', '_'):gsub('+_', ''):lower()
    local result = {
        name = 'cb.' .. bench_key .. '.' .. chart_name,
        param = tostring(math.floor(value)), unit = 'rps',
        tab = 'cbench.' .. chart_name, v = version
    }
    print(urlencode(result))
end

function run()
    local benches = bench.run(workloads, count, iterations, engine)
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
