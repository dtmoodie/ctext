cmake_minimum_required(VERSION 3.5)

project(ctext)
option(BUILD_TESTS "Build tests" ON)

find_package(ct REQUIRED)
find_package(minitensor REQUIRED)


add_library(ctext INTERFACE)
target_include_directories(ctext
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(ctext
    INTERFACE
        ct
        minitensor
)


if(BUILD_TESTS)
    add_subdirectory(tests)
endif(BUILD_TESTS)
