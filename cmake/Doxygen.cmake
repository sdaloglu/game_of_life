function(Doxygen input ouput)
    find_package (Doxygen)
    
    if (NOT DOXYGEN_FOUND)
        add_custom_target(doxygen COMMAND False
            COMMENT "Doxygen not found"
        )
        return()
    endif()

    # Ensure the correct output directory by setting it relative to PROJECT_SOURCE_DIR
    set(DOXYGEN_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/docs)

    # Specify the config file
    set(DOXYGEN_CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)

    if(NOT EXISTS ${DOXYGEN_CONFIG_FILE})
        message(FATAL_ERROR "Doxygen config file 'Doxyfile' not found at ${DOXYGEN_CONFIG_FILE}")
    endif()

    # Additional configuration options
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_LATEX YES)

  
    set(DOXYGEN_QUIET YES)

    doxygen_add_docs(doxygen 
        ${DOXYGEN_CONFIGURED_INPUT}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Generating doxygen documentation"
    )

endfunction()