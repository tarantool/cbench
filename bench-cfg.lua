#!/usr/bin/env tarantool

box.cfg {
    slab_alloc_arena    = 1,
    pid_file            = "tarantool.pid",
    wal_mode            = "none",
    snap_dir = ".",
    work_dir = "."
}

-- Tests to run
tests = {'replaces', 'selects', 'selrepl', 'updates', 'deletes'}
-- Workloads
workloads = {
    -- Run one extra test to warm up the server
    {tests = tests, type = 'hash', parts = { 'num'}},
    {tests = tests, type = 'hash', parts = { 'num' }},
    {tests = tests, type = 'hash', parts = { 'str' }},
--[[
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

local bench = require('cbench')
local json = require('json')

print('Benchmarking...')
-- Run benchmark
result = bench.run(workloads, 1000000, 5);
print('Done')

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
