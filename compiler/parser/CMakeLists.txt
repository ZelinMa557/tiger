add_library(parser)

target_sources(parser
  PRIVATE
    parser.cpp
  PUBLIC
    ast.h
    parser.h
  )