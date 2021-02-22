cmake_minimum_required(VERSION 3.16)
project(SimpleClient)

set(CMAKE_CXX_STANDARD 17)

add_executable(SimpleClient main.cpp threadsafe_queue.cpp threadsafe_queue.h)

include_directories("D:\\Dev\\CPP\\vcpkg\\installed\\x86-windows\\include")
target_link_libraries(${CMAKE_PROJECT_NAME} ws2_32)