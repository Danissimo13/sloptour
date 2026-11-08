find_package(GTest REQUIRED)

sloptour_add_executable(sloptour_tests
    DEPENDENCIES
        sloptour_terminal
        sloptour_infrastructure
        GTest::gtest_main
)

target_include_directories(sloptour_tests PRIVATE "${PROJECT_SOURCE_DIR}")

include(GoogleTest)
gtest_discover_tests(sloptour_tests)
