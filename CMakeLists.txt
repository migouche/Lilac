cmake_minimum_required(VERSION 3.01)

project(Lilac)

set(CMAKE_CXX_STANDARD 23)
#if linux then add this for better autocompletion when writing tests
if(UNIX AND NOT APPLE)
    include_directories(~/local/include)
endif ()

include_directories(include)

file(GLOB HEADERS "include/**/*.h")
file(GLOB SOURCES "src/**/*.cpp")


add_executable(lilac src/main.cpp ${SOURCES} ${HEADERS}
        src/parser/AST/functiondeclaration.cpp) #:(


