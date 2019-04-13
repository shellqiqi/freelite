cmake_minimum_required(VERSION 3.0.0)
project(freelite VERSION 0.1.0)

# INFO            LOG_LEVEL=2
# DEBUG           LOG_LEVEL=1
# WARNING & ERROR LOG_LEVEL=0
add_definitions(-DLOG_LEVEL=1)

# add_definitions(-DNO_CONTAINER)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

ADD_SUBDIRECTORY(client)
ADD_SUBDIRECTORY(router)
ADD_SUBDIRECTORY(vcm)

