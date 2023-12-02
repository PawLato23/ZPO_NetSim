cmake_minimum_required(VERSION 3.24)
project(NetSim)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Werror -Wpedantic -pedantic-errors -Wconversion)

include_directories(
        include
)

set(SOURCE_FILES
        src/package.cpp
        src/storage_types.cpp
        )

add_executable(NetSim ${SOURCE_FILES} main.cpp)
