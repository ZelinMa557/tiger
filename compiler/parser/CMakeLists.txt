add_library(parser)

target_sources(parser
  PRIVATE
    parser.cpp
  )

target_link_libraries(parser lexer)