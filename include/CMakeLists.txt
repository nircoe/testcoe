add_library(testcoe_headers INTERFACE)
target_include_directories(testcoe_headers
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

install(
    DIRECTORY testcoe
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

install(
    TARGETS testcoe_headers
    EXPORT testcoe_targets
)