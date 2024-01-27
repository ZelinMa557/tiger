add_executable(
    compiler_unit_test
    lexer_test.cpp
)

target_link_libraries(compiler_unit_test
  PRIVATE
    lexer
    gtest
    gtest_main
)