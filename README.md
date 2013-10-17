Tarantool C Bench
=================

## Overview

Tarantool C Bench is a **simple tool to benchmark Tarantool internal API**. Tarantool (http://tarantool.org) is an efficient in-memory NoSQL database and a Lua application server, blended. 

The tool is used internally by Tarantool team to check for performance regressions during development cycle. All workloads are written in C++. Lua (FFI) is only used to load & run workloads and display results (does not affect performance).

## Compatibility

+ Tarantool 1.5.1+ (including 1.6.x)

## Installation

Since the module uses internal API which is not exported to public headers you need entire Tarantool source tree to build a binary library.

    ~ $ git clone --recursive https://github.com/tarantool/tarantool.git tarantool
    ~ $ cd tarantool

Compile Tarantool in Release mode:

    ~/tarantool/ $ cmake . -DCMAKE_BUILD_TYPE=Release
    ~/tarantool/ $ make -j8

Get the module:

    ~/tarantool/ $ cd test
    ~/tarantool/test $ git clone https://github.com/rtsisyk/tarantool-c-bench.git var

Build the module:

    ~/tarantool/test $ cd var
    ~/tarantool/test/var $ cmake . -DCMAKE_BUILD_TYPE=Release
    ~/tarantool/test/var $ make

Please note that the compiled binary library may be incompatible with other builds of Tarantool.

## Usage

You can find sample `tarantool.cfg` and `init.lua` files in the root tree of the module. Please review server settings in `tarantool.cfg` and workload configuration in `example.lua`:

    ~/tarantool/test/var $ ${EDITOR} tarantool.cfg
    ~/tarantool/test/var $ ${EDITOR} init.lua

With Tarantool 1.5 and older the data storage must be initialized explicitly:

    ~/tarantool/test/var $ ../../src/box/tarantool_box --init-storage # only for 1.5

Start Tarantool to get benchmark results:

    ~/tarantool/test/var $ ../../src/box/tarantool_box

Results are printed to the console and `tarantool.log`:

```
2013-10-17 11:55:38.277 [26385] 101/init.lua I> ----------------------------------
2013-10-17 11:55:39.309 [26385] 101/init.lua I> HASH NUM
2013-10-17 11:55:39.309 [26385] 101/init.lua I> ----------------------------------
2013-10-17 11:55:39.309 [26385] 101/init.lua I> replaces  : 1706523.16  rps
2013-10-17 11:55:39.309 [26385] 101/init.lua I> selects   : 2102669.76  rps
2013-10-17 11:55:39.309 [26385] 101/init.lua I> deletes   : 1690311.04  rps
2013-10-17 11:55:39.309 [26385] 101/init.lua I> ----------------------------------
2013-10-17 11:55:41.933 [26385] 101/init.lua I> HASH STR
2013-10-17 11:55:41.933 [26385] 101/init.lua I> ----------------------------------
2013-10-17 11:55:41.933 [26385] 101/init.lua I> replaces  : 1250570.18  rps
2013-10-17 11:55:41.933 [26385] 101/init.lua I> selects   : 1528495.59  rps
2013-10-17 11:55:41.933 [26385] 101/init.lua I> deletes   : 1150491.84  rps
2013-10-17 11:55:41.933 [26385] 101/init.lua I> ----------------------------------
2013-10-17 11:56:33.537 [26385] 101/init.lua I> Benchmark result saved to bench-result-1.6.0-203-ge116619-Linux-x86_64-Debug.json
```

A provided `init.lua` also saves results in JSON format into a file in the current directory:

     ~/tarantool/test/var $ cat *.json
    [["HASH NUM",[["replaces",1706523.160216],["selects",2102669.7558844],["deletes",1690311.0366006]]],["HASH STR",[["replaces",1250570.1797105],["selects",1528495.5941411],["deletes",1150491.8403015]]]]


## Feedback

Please report bugs at http://github.com/tarantool/tarantool/issues We also warmly welcome your feedback in the discussion mailing list, tarantool@googlegroups.com.

Thank you for your interest in Tarantool!
