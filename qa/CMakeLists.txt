 cmake_minimum_required(VERSION 2.8.7)
 PROJECT(monkey)
 #add_definitions("-O3 -g  -pg -ggdb -Wunreachable-code")
 add_definitions("-O3 -g -Wunreachable-code")
 #//set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O3 -g")
 #set(CMAKE_CXX_FLAGS_RELEASE " -O3 -Wall -Wextra")
 set(CMAKE_CXX_STANDARD 14)

 INCLUDE_DIRECTORIES(..)
 set(HEADERS
     ../argparser.hpp
     ../auxiliaries.hpp
     ../token.hpp
     ../lexer.hpp
     ../symbols.hpp
     ../object.hpp
     ../visitor.hpp
     ../builtins.hpp
     ../ast.hpp
     ../parser.hpp
     ../pool.hpp
     ../env.hpp
     test.hpp
    )
 set(SOURCES
     ../argparser.cpp
     ../token.cpp
     ../lexer.cpp
     ../object.cpp
     ../builtins.cpp
     ../symbols.cpp
     ../ast.cpp
     ../visitor.cpp
     ../parser.cpp
     ../env.cpp
     test.cpp
     main.cpp
     )
 ADD_EXECUTABLE(qa_monkey ${HEADERS} ${SOURCES})

