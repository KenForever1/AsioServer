cmake_minimum_required(VERSION 3.15)
project(AsioServer)

set(CMAKE_CXX_STANDARD 11)

add_executable(AsioServer main.cpp RWHandler.hpp Server.hpp)
target_link_libraries(AsioServer pthread)