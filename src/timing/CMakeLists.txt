add_library(timing_lib
    STATIC 
    timing.cpp
)

ClangTidy(timing_lib)

target_include_directories(timing_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
