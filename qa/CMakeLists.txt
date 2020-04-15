 cmake_minimum_required(VERSION 2.8.7)
 PROJECT(monkey)
 add_definitions("-pg -ggdb -Wunreachable-code")
 set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
 set(CMAKE_CXX_FLAGS_RELEASE " -O3 -Wall -Wextra")
 set(CMAKE_CXX_STANDARD 14)

 INCLUDE_DIRECTORIES(..)
 set(HEADERS
     ../argparser.hpp
     ../auxiliaries.hpp
     ../token.hpp
     ../lexer.hpp
     ../symbols.hpp
     ../ast.hpp
     ../parser.hpp
     ../pool.hpp
     test.hpp
    )
 set(SOURCES
     ../argparser.cpp
     ../token.cpp
     ../lexer.cpp
     ../symbols.cpp
     ../ast.cpp
     ../parser.cpp
     test.cpp
     main.cpp
     )
 ADD_EXECUTABLE(qa_monkey ${HEADERS} ${SOURCES})

