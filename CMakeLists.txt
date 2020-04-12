 cmake_minimum_required(VERSION 2.8.7)
 PROJECT(monkey)
 #$//add_definitions("-Wall -Wextra -pg -ggdb -O0 -Wunreachable-code")
 set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
 set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wall -Wextra")
 set(CMAKE_CXX_STANDARD 14)
 set(HEADERS
     token.hpp
    )
 set(SOURCES
     repl.cpp
     )
 ADD_EXECUTABLE(monkey ${HEADERS} ${SOURCES})

