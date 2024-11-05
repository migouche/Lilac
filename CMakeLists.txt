cmake_minimum_required(VERSION 3.11.0)

project(Lilac)

#if linux then add this for better autocompletion when writing tests
if(UNIX AND NOT APPLE)
    include_directories(~/local/include)
endif ()

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(LLVM_TARGETS_TO_BUILD "X86;x64" CACHE STRING "" FORCE)
set(DLLVM_USE_LINDER lld)

include_directories(include)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Link against LLVM libraries

# target_link_libraries(simple-tool ${llvm_libs}) # Removed this line

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp include/*.h)

set(CMAKE_CXX_STANDARD 23)

add_link_options(-fsanitize=address)


add_executable(lilac src/main.cpp ${SOURCE_FILES} )

llvm_map_components_to_libnames(LLVM_LIBS
        core
        support
        irreader
        target
        mc
        X86
        # Add other required components here
)

# target_link_libraries(lilac llvm) # Removed this line
target_link_libraries(lilac ${LLVM_LIBS}) # Added this line