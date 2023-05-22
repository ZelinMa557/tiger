cmake_minimum_required(VERSION 3.10)
project(Tiger)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

SET(CMAKE_BUILD_TYPE "Debug") 
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -g2 -ggdb")

# include llvm
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs support core irreader)

include_directories(include)
set(LEXER_SRC src/lexer.cpp)
set(PARSER_SRC src/parser.cpp)
set(CHECKER_SRC src/symtbl.cpp src/tychecker.cpp)
set(GENERATOR_SRC src/generator.cpp src/MyPointerType.hpp)
set(MAIN_SRC src/main.cpp)

add_executable(lexer_test ${LEXER_SRC} test/lexer_test.cpp)
add_executable(parser_test ${LEXER_SRC} ${PARSER_SRC} src/printabs.cpp test/parser_test.cpp)
add_executable(checker_test ${LEXER_SRC} ${PARSER_SRC} ${CHECKER_SRC} src/printabs.cpp test/tychecker_test.cpp)

add_executable(tiger ${MAIN_SRC} ${LEXER_SRC} ${PARSER_SRC} ${CHECKER_SRC} ${GENERATOR_SRC})
target_link_libraries(tiger ${llvm_libs})