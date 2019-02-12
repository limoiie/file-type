add_executable(all_test_cases
        test-encoding.cpp
        )

target_link_libraries(all_test_cases gtest_main)
add_test(NAME test_all_cases COMMAND all_test_cases)