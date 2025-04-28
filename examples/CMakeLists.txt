add_subdirectory(basic)
add_subdirectory(crash)
add_subdirectory(filter)

add_custom_target(build_all_examples
    DEPENDS 
        basic_example
        crash_example
        filtering_example
)

# Add a target to run basic examples (those that don't crash)
add_custom_target(run_all_examples
    DEPENDS
        run_basic_example
        run_filtering_example
)