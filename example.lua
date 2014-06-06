#!../../src/tarantool

local bench = require "bench"

-- Tests to run
tests = {'replaces', 'selects', 'updates', 'deletes'}
-- Workloads
workloads = {
    -- Run one extra test to warm up the server
    {tests = tests, type = 'hash', parts = { 0, 'num'}},
    {tests = tests, type = 'hash', parts = { 0, 'num'}},
    {tests = tests, type = 'hash', parts = { 0, 'str'}},
    {tests = tests, type = 'hash', parts = { 0, 'num', 1, 'num'}},
    {tests = tests, type = 'hash', parts = { 0, 'num', 1, 'str'}},
    {tests = tests, type = 'hash', parts = { 0, 'str', 1, 'num'}},
    {tests = tests, type = 'hash', parts = { 0, 'str', 1, 'str'}},
    {tests = tests, type = 'tree', parts = { 0, 'num'}},
    {tests = tests, type = 'tree', parts = { 0, 'str'}},
    {tests = tests, type = 'tree', parts = { 0, 'num', 1, 'num'}},
    {tests = tests, type = 'tree', parts = { 0, 'num', 1, 'str'}},
    {tests = tests, type = 'tree', parts = { 0, 'str', 1, 'num'}},
    {tests = tests, type = 'tree', parts = { 0, 'str', 1, 'str'}}
}

-- Run benchmark
result = bench.run(workloads, 1000000, 5);

-- Find json
local json
if box.info.version >= "1.6.0" then
    json = require("json")
else
    json = box.cjson
end

-- Encode the result and save to a file
json_result = json.encode(result)
filename = string.format('bench-result-%s-%s.json',
    box.info.version, box.info.build.target);
file = io.open(filename, 'w')
file:write(json_result)
file:flush()
file:close()
print('Benchmark result saved to ', filename)

