function(testcoe_generate_config_header)
    set(TESTCOE_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/config")
    set(TESTCOE_CONFIG_DIR ${TESTCOE_CONFIG_DIR} PARENT_SCOPE)

    file(MAKE_DIRECTORY ${TESTCOE_CONFIG_DIR})

    configure_file(
        ${PROJECT_SOURCE_DIR}/cmake/testcoe_config.hpp.in
        ${TESTCOE_CONFIG_DIR}/testcoe_config.hpp
        @ONLY
    )
endfunction()
