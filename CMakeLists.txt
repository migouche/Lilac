cmake_minimum_required(VERSION 3.11.0)

project(Lilac)

#if linux then add this for better autocompletion when writing tests
if(UNIX AND NOT APPLE)
    include_directories(~/local/include)
endif ()

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(include)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs support core irreader)
# target_link_libraries(simple-tool ${llvm_libs}) # Removed this line

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp include/*.h)

set(CMAKE_CXX_STANDARD 23)

add_link_options(-fsanitize=address)


add_executable(lilac src/main.cpp ${SOURCE_FILES} )

# target_link_libraries(lilac llvm) # Removed this line
target_link_libraries(lilac ${llvm_libs}) # Added this line