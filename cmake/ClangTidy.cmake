if(NOT SLOPTOUR_ENABLE_CLANG_TIDY)
    return()
endif()

find_program(SLOPTOUR_CLANG_TIDY_EXECUTABLE
    NAMES clang-tidy
    HINTS /opt/homebrew/opt/llvm/bin /usr/local/opt/llvm/bin
)

if(NOT SLOPTOUR_CLANG_TIDY_EXECUTABLE)
    message(FATAL_ERROR "clang-tidy not found. Install LLVM or configure with -DSLOPTOUR_ENABLE_CLANG_TIDY=OFF")
endif()

set(SLOPTOUR_CLANG_TIDY_COMMAND "${SLOPTOUR_CLANG_TIDY_EXECUTABLE}")

if(NOT SLOPTOUR_CLANG_TIDY_ANALYZER)
    list(APPEND SLOPTOUR_CLANG_TIDY_COMMAND "--checks=-clang-analyzer-*")
endif()

if(APPLE AND CMAKE_OSX_SYSROOT)
    list(APPEND SLOPTOUR_CLANG_TIDY_COMMAND "--extra-arg=-isysroot${CMAKE_OSX_SYSROOT}")
endif()
