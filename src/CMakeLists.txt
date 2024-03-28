find_package(MPI REQUIRED)
add_subdirectory(timing)


# Creating a library for grid.cpp
add_library(libgrid
    grid.cpp
    grid_utils.cpp
)
target_include_directories(libgrid
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${MPI_C_INCLUDE_PATH}
    )

target_link_libraries(libgrid PUBLIC MPI::MPI_CXX)

# Creating game executable for the main.cpp
add_executable(game 
    main.cpp 
   
)

# Directories to include when compiling the game (header files)
target_include_directories(game
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Linking the game with the grid library as well as the timing library
target_link_libraries(game PUBLIC libgrid timing_lib)


# Clang-tidy
#ClangTidy(game)
# Clang-format
#ClangFormat(game .)