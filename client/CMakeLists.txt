aux_source_directory(lib DIR_CLIENT_LIB_SRCS)

add_library(CLIENT_LIB ${DIR_CLIENT_LIB_SRCS})

add_executable(test_client test_client.c)
add_executable(test_server test_server.c)

target_link_libraries(test_client CLIENT_LIB rt)
target_link_libraries(test_server Threads::Threads)

add_executable(test_join test_join.c)
add_executable(test_write test_write.c)
add_executable(test_latency test_latency.c)
add_executable(test_rpc test_rpc.c)

target_link_libraries(test_join CLIENT_LIB rt)
target_link_libraries(test_write CLIENT_LIB rt)
target_link_libraries(test_latency CLIENT_LIB rt Threads::Threads)
target_link_libraries(test_rpc CLIENT_LIB rt Threads::Threads)

