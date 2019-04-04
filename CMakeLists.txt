cmake_minimum_required(VERSION 3.0.0)
project(freelite VERSION 0.1.0)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

aux_source_directory(lib DIR_LIB_SRCS)

add_library(LITE_LIB ${DIR_LIB_SRCS})

add_executable(test_client test_client.c)
add_executable(test_server test_server.c)

add_executable(test_join test_join.c)

target_link_libraries(test_client LITE_LIB)
target_link_libraries(test_join LITE_LIB)

target_link_libraries(test_server Threads::Threads)

