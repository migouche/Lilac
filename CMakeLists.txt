cmake_minimum_required(VERSION 3.01)

project(Lilac)

set(CMAKE_CXX_STANDARD 23)
#if linux then add this for better autocompletion when writing tests
if(UNIX AND NOT APPLE)
    include_directories(~/local/include)
endif ()

include_directories(include)

add_executable(lilac src/main.cpp include/lexer/filestream.h src/lexer/filestream.cpp include/lexer/token.h src/lexer/token.cpp include/typedefs.h
        include/lexer/tokenizer.h
        src/lexer/tokenizer.cpp)

