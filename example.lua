#!../../src/tarantool

package.path = '?.lua' -- A workaround on Bug#38
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
