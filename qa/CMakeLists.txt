 cmake_minimum_required(VERSION 2.8.7)
 PROJECT(monkey)

 set(CMAKE_CXX_STANDARD 11)
 set(CMAKE_CXX_STANDARD_REQUIRED True)
 #set(CMAKE_BUILD_TYPE Release)

 SET(ROOT_DIR ..)
 INCLUDE_DIRECTORIES(${ROOT_DIR})

 set(HEADERS
     ${ROOT_DIR}/argparser.hpp
     ${ROOT_DIR}/auxiliaries.hpp
     ${ROOT_DIR}/token.hpp
     ${ROOT_DIR}/lexer.hpp
     ${ROOT_DIR}/symbols.hpp
     ${ROOT_DIR}/object.hpp
     ${ROOT_DIR}/builtins.hpp
     ${ROOT_DIR}/ast.hpp
     ${ROOT_DIR}/parser.hpp
     ${ROOT_DIR}/pool.hpp
     ${ROOT_DIR}/env.hpp
     ${ROOT_DIR}/visitor.hpp
     test.hpp
    )
 set(SOURCES
     ${ROOT_DIR}/argparser.cpp
     ${ROOT_DIR}/token.cpp
     ${ROOT_DIR}/lexer.cpp
     ${ROOT_DIR}/object.cpp
     ${ROOT_DIR}/builtins.cpp
     ${ROOT_DIR}/symbols.cpp
     ${ROOT_DIR}/ast.cpp
     ${ROOT_DIR}/parser.cpp
     ${ROOT_DIR}/env.cpp
     ${ROOT_DIR}/visitor.cpp
     test.cpp
     main.cpp
     )
 ADD_EXECUTABLE(qa_monkey ${HEADERS} ${SOURCES})

