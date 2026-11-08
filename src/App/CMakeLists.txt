sloptour_add_executable(sloptour
    DEPENDENCIES
        sloptour_terminal
        sloptour_infrastructure
        CLI11::CLI11
)

install(TARGETS sloptour RUNTIME DESTINATION bin)
