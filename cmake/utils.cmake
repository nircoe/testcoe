# Automatically copies required MinGW DLLs to target directory on Windows

function(copy_mingw_dlls_to_target target_name)
    if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(STATUS "[testcoe] Setting up automatic MinGW DLL copying for ${target_name}")
        
        # Get the compiler directory
        get_filename_component(COMPILER_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
        
        # Also check the parent bin directory (common in MinGW installations)
        get_filename_component(MINGW_ROOT ${COMPILER_DIR} DIRECTORY)
        set(MINGW_BIN_DIR "${MINGW_ROOT}/bin")
        
        # List of DLLs that might be needed for MinGW
        set(MINGW_DLLS
            libgcc_s_seh-1.dll     # 64-bit SEH exception handling
            libgcc_s_dw2-1.dll     # 32-bit Dwarf2 exception handling
            libstdc++-6.dll        # C++ standard library
            libwinpthread-1.dll    # Threading support
            libgomp-1.dll          # OpenMP support (sometimes needed)
        )
        
        # Search paths for DLLs
        set(SEARCH_PATHS 
            ${COMPILER_DIR}
            ${MINGW_BIN_DIR}
        )
        
        foreach(dll ${MINGW_DLLS})
            set(dll_found FALSE)
            foreach(search_path ${SEARCH_PATHS})
                set(dll_path "${search_path}/${dll}")
                if(EXISTS "${dll_path}")
                    message(STATUS "[testcoe] Found ${dll} at ${dll_path}")
                    add_custom_command(TARGET ${target_name} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${dll_path}"
                        "$<TARGET_FILE_DIR:${target_name}>/${dll}"
                        COMMENT "[testcoe] Copying MinGW DLL: ${dll}"
                        VERBATIM
                    )
                    set(dll_found TRUE)
                    break()
                endif()
            endforeach()
            
            if(NOT dll_found)
                message(STATUS "[testcoe] ${dll} not found in any search path, skipping")
            endif()
        endforeach()
        
        # Also copy any DLLs that are in the same directory as the compiler
        # This is a more comprehensive approach
        message(STATUS "[testcoe] Adding compiler directory to PATH for runtime: ${COMPILER_DIR}")
        
        # Create a batch file that sets the PATH and runs the executable
        set(WRAPPER_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/${target_name}_with_path.bat")
        file(GENERATE OUTPUT "${WRAPPER_SCRIPT}"
            CONTENT "@echo off\nset PATH=${COMPILER_DIR};${MINGW_BIN_DIR};%PATH%\n\"$<TARGET_FILE:${target_name}>\" %*\n"
        )
        
    endif()
endfunction()

# Alternative function for multiple targets
function(copy_mingw_dlls_to_targets)
    foreach(target_name ${ARGN})
        copy_mingw_dlls_to_target(${target_name})
    endforeach()
endfunction()

function(copy_mingw_dlls_to_directory target_dir)
    if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(STATUS "[testcoe] Copying MinGW DLLs to directory: ${target_dir}")
        
        get_filename_component(COMPILER_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
        get_filename_component(MINGW_ROOT ${COMPILER_DIR} DIRECTORY)
        set(MINGW_BIN_DIR "${MINGW_ROOT}/bin")
        
        set(MINGW_DLLS
            libgcc_s_seh-1.dll
            libgcc_s_dw2-1.dll
            libstdc++-6.dll
            libwinpthread-1.dll
            libgomp-1.dll
        )
        
        set(SEARCH_PATHS 
            ${COMPILER_DIR}
            ${MINGW_BIN_DIR}
        )
        
        foreach(dll ${MINGW_DLLS})
            foreach(search_path ${SEARCH_PATHS})
                set(dll_path "${search_path}/${dll}")
                if(EXISTS "${dll_path}")
                    message(STATUS "[testcoe] Copying ${dll} to ${target_dir}")
                    file(COPY "${dll_path}" DESTINATION "${target_dir}")
                    break()
                endif()
            endforeach()
        endforeach()
    endif()
endfunction()

function(ignore_external_warnings target_name)
    if(TARGET ${target_name})
        get_target_property(INCLUDE_DIRS ${target_name} INTERFACE_INCLUDE_DIRECTORIES)
        if(INCLUDE_DIRS)
            set_target_properties(${target_name} PROPERTIES
                INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${INCLUDE_DIRS}"
            )
            message(STATUS "Ignoring ${target_name} warnings")
        endif()
    else()
        message(WARNING "Target ${target_name} not found")
    endif()
endfunction()