cmake_minimum_required(VERSION 3.0.0)
project(freelite VERSION 0.1.0)

add_library(lite-lib lite-lib.c)
add_library(rpc rpc.c)

add_executable(test_client test_client.c)
add_executable(test_server test_server.c)

target_link_libraries(test_client lite-lib rpc)

