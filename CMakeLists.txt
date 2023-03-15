cmake_minimum_required(VERSION 3.10)
project(Tiger)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

add_executable(lexer_test src/lexer.cpp test/lexer_test.cpp)