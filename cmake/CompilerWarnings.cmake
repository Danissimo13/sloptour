add_library(sloptour_warnings INTERFACE)

if(MSVC)
    target_compile_options(sloptour_warnings INTERFACE
        /W4
        /permissive-
        $<$<BOOL:${SLOPTOUR_WARNINGS_AS_ERRORS}>:/WX>
    )
else()
    target_compile_options(sloptour_warnings INTERFACE
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
        -Wsign-conversion
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
        -Wnull-dereference
        -Wimplicit-fallthrough
        $<$<BOOL:${SLOPTOUR_WARNINGS_AS_ERRORS}>:-Werror>
    )
endif()
