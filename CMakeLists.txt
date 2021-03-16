cmake_minimum_required(VERSION 3.17)
project(libfixeypointy)

set(CMAKE_CXX_STANDARD 17)

add_library(libfixeypointy src/decimal.h src/decimal.cpp src/magic_numbers.h)

add_executable(demo src/demo.cpp)
target_link_libraries(demo libfixeypointy)