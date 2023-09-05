cmake_minimum_required(VERSION 3.25)

project(Lilac)

set(CMAKE_CXX_STANDARD 23)
#if linux then add this for better autocompletion when writing tests
if(UNIX AND NOT APPLE)
    include_directories(~/local/include)
endif ()

include_directories(include)

add_executable(lilac src/main.cpp include/lexer/filestream.h src/lexer/filestream.cpp)
