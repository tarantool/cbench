Tarantool C Bench
=================

## Overview

Tarantool C Bench is a **simple tool to benchmark Tarantool internal API**. Tarantool (http://tarantool.org) is an efficient in-memory NoSQL database and a Lua application server, blended.

The tool is used internally by Tarantool team to check for performance regressions during development cycle. All workloads are written in C++. Lua (FFI) is only used to load & run workloads and display results (does not affect performance).

[![Build Status](https://travis-ci.org/tarantool/cbench.png?branch=master)](https://travis-ci.org/tarantool/cbench)

## Prerequisites

 * Tarantool 1.6.5+ with header files (tarantool && tarantool-dev packages)
 * libmsgpuck header files (libmsgpuck-dev package)

## Installation

Clone repository and then build it using CMake:

``` bash
git clone https://github.com/tarantool/cbench.git tarantool-cbench
cd tarantool-cbench && cmake . -DCMAKE_BUILD_TYPE=RelWithDebugInfo
make
make install
```

## Usage

Modify workload configuration in bench-cfg.lua and run it:

    $ ./bench-cfg.lua

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

A provided script also saves results in JSON format into a file in the current directory:

     ~/tarantool/test/var $ cat *.json
    [["HASH NUM",[["replaces",1706523.160216],["selects",2102669.7558844],["deletes",1690311.0366006]]],["HASH STR",[["replaces",1250570.1797105],["selects",1528495.5941411],["deletes",1150491.8403015]]]]


## Feedback

Please report bugs at http://github.com/tarantool/tarantool/issues We also warmly welcome your feedback in the discussion mailing list, tarantool@googlegroups.com.

Thank you for your interest in Tarantool!
