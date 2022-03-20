cmake_minimum_required(VERSION 3.5)

project(ctext)
option(BUILD_TESTS "Build tests" ON)

if(NOT TARGET ct)
    find_package(ct QUIET)
endif()

find_package(minitensor QUIET)
if(NOT TARGET minitensor)
    add_subdirectory(dependencies/minitensor)
endif()


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

export(TARGETS ctext
    FILE ctext-targets.cmake
)

install(TARGETS ctext
      EXPORT ctext
      DESTINATION lib
) # ignored

install(EXPORT ctext FILE ctext-config.cmake
    DESTINATION share/ctext
)
install(DIRECTORY include/ctext DESTINATION include)


if(BUILD_TESTS)
    add_subdirectory(tests)
endif(BUILD_TESTS)
