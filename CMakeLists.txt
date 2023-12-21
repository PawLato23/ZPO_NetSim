cmake_minimum_required(VERSION 3.24)
project(NetSim)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Werror -Wpedantic -pedantic-errors -Wno-error=pessimizing-move)

include_directories(
        include
)

set(SOURCE_FILES
        src/package.cpp
        src/storage_types.cpp
        src/helpers.cpp
        src/nodes.cpp
        src/factory.cpp
        )

add_executable(NetSim ${SOURCE_FILES} main.cpp)

# Google Testing Framework

set(SOURCES_FILES_TESTS
        test/test_Factory.cpp
        #test/test_factory_io.cpp
        test/test_nodes.cpp
        test/test_package.cpp
        #test/test_reports.cpp
        #test/test_simulate.cpp
        test/test_storage_types.cpp
        )

add_subdirectory(googletest-master)

set(EXECUTABLE_NAME NetSim_factory)
add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES} ${SOURCES_FILES_TESTS} test/main_gtest.cpp)
target_compile_definitions(${EXECUTABLE_NAME} PUBLIC EXERCISE_ID=EXERCISE_ID_FACTORY)

target_include_directories(${EXECUTABLE_NAME} PUBLIC
        googletest-master/googlemock/include
        googletest-master/googletest/include
        mocks
        )

target_link_libraries(${EXECUTABLE_NAME} gmock)
