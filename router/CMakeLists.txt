aux_source_directory(lib DIR_ROUTER_LIB_SRCS)

add_library(ROUTER_LIB ${DIR_ROUTER_LIB_SRCS})

add_executable(test_router test_router.c)

target_link_libraries(test_router Threads::Threads ROUTER_LIB)

