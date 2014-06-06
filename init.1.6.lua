#!../../src/tarantool

box.cfg {
    primary_port        = 33333,
    admin_port          = 33334,
    slab_alloc_arena    = 1,
    pid_file            = "tarantool.pid",
    wal_mode            = "none",
    rows_per_wal        = 1000000,
    snap_dir = ".",
    log_dir = "."
}

dofile('example.lua')

json = require("json")

-- Encode the result and save to a file
json_result = json.encode(result)
filename = string.format('bench-result-%s-%s.json',
    box.info.version, box.info.build.target);
file = io.open(filename, 'w')
file:write(json_result)
file:flush()
file:close()
print('Benchmark result saved to ', filename)

box.raise(0, "finish")
