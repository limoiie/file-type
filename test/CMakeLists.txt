configure_file (
        "${PROJECT_SOURCE_DIR}/test/test-config.h.in"
        "${PROJECT_BINARY_DIR}/test/test-config.h"
)

include_directories("${PROJECT_BINARY_DIR}/test")

add_executable(all_test_cases
        test-encoding.cpp
        ../encodings.cpp
        ../encodings.h
        )

target_link_libraries(all_test_cases gtest_main)
add_test(NAME test_all_cases COMMAND all_test_cases)