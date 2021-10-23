file(GLOB hdr "*.hpp")

add_executable(test_ctext_data_table ${hdr} datatable.cpp)
target_link_libraries(test_ctext_data_table 
    ct 
    ctext
    gtest
    gtest_main
    pthread
    minitensor
)
target_include_directories(test_ctext_data_table SYSTEM PRIVATE ${GTEST_INCLUDE})
set_target_properties(test_ctext_data_table PROPERTIES FOLDER Tests/ct)
add_test(test_ctext_data_table test_ctext_data_table)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    target_compile_options(test_ctext_data_table PRIVATE -Werror -Wall -Wextra)
endif()
