cmake_minimum_required(VERSION 3.11.0)

project(Lilac)

set(CMAKE_CXX_STANDARD 23)
#if linux then add this for better autocompletion when writing tests
if(UNIX AND NOT APPLE)
    include_directories(~/local/include)
endif ()

include_directories(include)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp include/*.h)


add_executable(lilac src/main.cpp ${SOURCE_FILES})