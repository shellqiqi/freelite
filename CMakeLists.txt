cmake_minimum_required(VERSION 3.0.0)
project(freelite VERSION 0.1.0)

#add_definitions(-DLOG_LEVEL=1)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

ADD_SUBDIRECTORY(client)
ADD_SUBDIRECTORY(router)

