#!../../src/box/tarantool_box

package.path = '?.lua' -- A workaround on Bug#38
local bench = require "bench"

-- Tests to run
tests = {'replaces', 'selects', 'deletes'}
-- Workloads
workloads = {
    -- Run some tests to warm up the server
    {tests = tests, type = 'hash', parts = { 0, 'num'}},
    {tests = tests, type = 'hash', parts = { 0, 'num'}},
    {tests = tests, type = 'hash', parts = { 0, 'str'}},
    {tests = tests, type = 'hash', parts = { 0, 'num', 1, 'str'}},
    {tests = tests, type = 'hash', parts = { 0, 'num', 1, 'str'}},
    {tests = tests, type = 'hash', parts = { 0, 'str', 1, 'num'}},
    {tests = tests, type = 'hash', parts = { 0, 'str', 1, 'str'}},
    {tests = tests, type = 'tree', parts = { 0, 'num'}},
    {tests = tests, type = 'tree', parts = { 0, 'str'}},
    {tests = tests, type = 'tree', parts = { 0, 'num', 1, 'str'}},
    {tests = tests, type = 'tree', parts = { 0, 'num', 1, 'str'}},
    {tests = tests, type = 'tree', parts = { 0, 'str', 1, 'num'}},
    {tests = tests, type = 'tree', parts = { 0, 'str', 1, 'str'}}
}

-- Run benchmark
local result = bench.run(workloads, 500000, 5);

-- Encode the result and save to a file
json_result = box.cjson.encode(result)
filename = string.format('bench-result-%s-%s.json',
    box.info.version, box.info.build.target);
file = io.open(filename, 'w')
file:write(json_result)
file:flush()
file:close()
print('Benchmark result saved to ', filename)
