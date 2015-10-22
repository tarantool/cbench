add_library(bench SHARED bench.c)
target_link_libraries(bench ${MSGPUCK_LIBRARIES})
set_target_properties(bench PROPERTIES PREFIX "" OUTPUT_NAME "bench")

# Install module
install(FILES init.lua DESTINATION ${TARANTOOL_LUADIR}/${PROJECT_NAME}/)
install(TARGETS bench LIBRARY DESTINATION ${TARANTOOL_LIBDIR}/${PROJECT_NAME}/)
