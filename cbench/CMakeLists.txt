if (APPLE)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined suppress -flat_namespace")
endif(APPLE)

add_library(bench SHARED bench.c)
target_link_libraries(bench ${MSGPUCK_LIBRARIES})
set_target_properties(bench PROPERTIES PREFIX "" OUTPUT_NAME "bench")

# Install module
install(FILES init.lua DESTINATION ${TARANTOOL_INSTALL_LUADIR}/${PROJECT_NAME}/)
install(TARGETS bench LIBRARY DESTINATION ${TARANTOOL_INSTALL_LIBDIR}/${PROJECT_NAME}/)
