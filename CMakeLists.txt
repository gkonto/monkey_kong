 cmake_minimum_required(VERSION 2.8.7)
 PROJECT(monkey)
 #add_definitions("-Wall -Wextra -ggdb -O0 -Wunreachable-code")
 # Set default compile flags for GCC

#if(CMAKE_COMPILER_IS_GNUCXX)
#   message(STATUS "GCC detected, adding compile flags")
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra")
#endif(CMAKE_COMPILER_IS_GNUCXX)
 

 #specify the C++ standard
 set(CMAKE_CXX_STANDARD 11)
 set(CMAKE_CXX_STANDARD_REQUIRED True)

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
     visitor.hpp
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
     visitor.cpp
     )
 ADD_EXECUTABLE(monkey ${HEADERS} ${SOURCES})

