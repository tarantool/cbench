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

box.raise(0, "finish")
