 cmake_minimum_required(VERSION 2.8.7)
 PROJECT(monkey)
 add_definitions("-Wall -Wextra -ggdb -O3 -Wunreachable-code")
 set(CMAKE_CXX_STANDARD 14)

 set(HEADERS
     token.hpp
     lexer.hpp
     repl.hpp
     argparser.hpp
     pool.hpp
     symbols.hpp
     object.hpp
     ast.hpp
     builtins.hpp
     env.hpp
     parser.hpp
    )

 set(SOURCES
     repl.cpp
     parser.cpp
     ast.cpp
     lexer.cpp
     object.cpp
     symbols.cpp
     argparser.cpp
     env.cpp
     token.cpp
     repl.cpp
     builtins.cpp
     )
 ADD_EXECUTABLE(monkey ${HEADERS} ${SOURCES})

