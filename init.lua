#!../../src/tarantool

if rawget(box, 'info') == nil then
    box.cfg {
        slab_alloc_arena    = 1,
        pid_file            = "tarantool.pid",
        wal_mode            = "none",
        snap_dir = ".",
        work_dir = "."
    }
end

-- Tests to run
tests = {'replaces', 'selects', 'selrepl', 'updates', 'deletes'}
-- Workloads
workloads = {
    -- Run one extra test to warm up the server
    {tests = tests, type = 'hash', parts = { 'num'}},
    {tests = tests, type = 'hash', parts = { 'num' }},
    {tests = tests, type = 'hash', parts = { 'str' }},
    {tests = tests, type = 'hash', parts = { 'num', 'num' }},
    {tests = tests, type = 'hash', parts = { 'num', 'str'}},
    {tests = tests, type = 'hash', parts = { 'str', 'num' }},
    {tests = tests, type = 'hash', parts = { 'str', 'str' }},
    {tests = tests, type = 'tree', parts = { 'num' }},
    {tests = tests, type = 'tree', parts = { 'str' }},
    {tests = tests, type = 'tree', parts = { 'num', 'num' }},
    {tests = tests, type = 'tree', parts = { 'num', 'str' }},
    {tests = tests, type = 'tree', parts = { 'str', 'num' }},
    {tests = tests, type = 'tree', parts = { 'str', 'str' }}
--]]
}

local bench = require "bench"
local json = box.cjson or require('json')

-- Run benchmark
result = bench.run(workloads, 1000000, 5);

-- Encode the result and save to a file
json_result = json.encode(result)
filename = string.format('bench-result-%s.json',
    box.info.version);
file = io.open(filename, 'w')
file:write(json_result)
file:flush()
file:close()
print('Benchmark result saved to ', filename)

os.exit(0)
