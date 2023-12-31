cmake_minimum_required(VERSION 3.18)

project(Circuit LANGUAGES CXX)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

find_package(Threads REQUIRED)

find_package(GTest REQUIRED)
enable_testing()

include_directories(./include
                    ./include/matrix/include
                    ./include/matrix/include/Vector/include)

set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

add_subdirectory(unit_tests)
add_subdirectory(include/matrix)
add_subdirectory(task)

add_custom_target(unit_tests 
    DEPENDS
        vector_test
        matrix_test
        circuit_test
)

add_custom_target(whole_project
    DEPENDS
        unit_tests
        currents
)
