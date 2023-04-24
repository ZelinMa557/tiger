cmake_minimum_required(VERSION 3.10)
project(Tiger)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

SET(CMAKE_BUILD_TYPE "Debug") 
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -g2 -ggdb")

include_directories(include)
set(LEXER_SRC src/lexer.cpp)
set(PARSER_SRC src/parser.cpp)
set(CHECKER_SRC src/symtbl.cpp src/tychecker.cpp)
add_executable(lexer_test ${LEXER_SRC} test/lexer_test.cpp)
add_executable(parser_test ${LEXER_SRC} ${PARSER_SRC} src/printabs.cpp test/parser_test.cpp)
add_executable(checker_test ${LEXER_SRC} ${PARSER_SRC} ${CHECKER_SRC} src/printabs.cpp test/tychecker_test.cpp)