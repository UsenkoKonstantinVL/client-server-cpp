
cmake_minimum_required(VERSION 3.5.1)
project(server_client_test_workout)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCES_SERVER src/servers.cpp)
set(SOURCES_CLIENT src/clients.cpp)


add_executable(main_server ${SOURCES_SERVER})
target_link_libraries(main_server)

add_executable(main_client ${SOURCES_CLIENT})
target_link_libraries(main_client)


