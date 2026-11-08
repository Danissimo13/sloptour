cmake_minimum_required(VERSION 3.26)

project(Sloptour VERSION 0.1.0 LANGUAGES CXX)

option(SLOPTOUR_BUILD_TESTS "Build unit tests" ON)
option(SLOPTOUR_ENABLE_CLANG_TIDY "Run clang-tidy during compilation" ON)
option(SLOPTOUR_CLANG_TIDY_ANALYZER "Include the slow clang-analyzer-* checks in clang-tidy" ON)
option(SLOPTOUR_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(MSVC)
    add_compile_options(/utf-8 /Zc:__cplusplus)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

include(CompilerWarnings)
include(ClangTidy)
include(SloptourModule)

find_package(ftxui REQUIRED)
find_package(reproc++ REQUIRED)
find_package(CLI11 REQUIRED)
find_package(nlohmann_json REQUIRED)

add_subdirectory(src)

if(SLOPTOUR_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
    sloptour_add_layer_rules_test()
endif()
