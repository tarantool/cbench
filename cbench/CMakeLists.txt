if (APPLE)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined suppress -flat_namespace")
endif(APPLE)

include_directories("../../" "../../src/" "../../third_party/luajit/src")
add_library(bench SHARED bench.c)
