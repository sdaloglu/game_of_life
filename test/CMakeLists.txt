include(FetchContent)
find_package(MPI REQUIRED)

if(NOT MPI_FOUND)
    message(FATAL_ERROR "MPI not found. Please ensure MPI is installed and MPI compilers are in your PATH.")
endif()

if(MPI_FOUND)
    include_directories(${MPI_C_INCLUDE_PATH})
endif()

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.13.0
)

set(gtest\_force\_shared\_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Create test executable
add_executable(test_grid test_grid.cpp)

# Ensure the MPI headers are available to test
target_include_directories(test_grid PUBLIC ${MPI_C_INCLUDE_PATH})

message(STATUS "MPI Include Path: ${MPI_C_INCLUDE_PATH}")  # Debugging

# Link the test executable with the grid, gtest and MPI libraries
target_link_libraries(test_grid 
            PRIVATE libgrid
            MPI::MPI_CXX
            GTest::gtest_main)

message(STATUS "MPI Libraries: ${MPI_C_LIBRARIES}")  # Debugging


include(GoogleTest)
gtest_discover_tests(test_grid)

