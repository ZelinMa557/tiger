add_library(lexer)

target_sources(lexer
  PRIVATE
    lexer.cpp
  PUBLIC
    lexer.h
    token.h
  )