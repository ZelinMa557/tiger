add_executable(
    lexer_test
    lexer_test.cpp
)

target_link_libraries(lexer_test
  PRIVATE
    lexer
    gtest
    gtest_main
)