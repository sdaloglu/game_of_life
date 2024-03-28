# Global CMakeLists.txt file for the GAME_OF_LIFE project

cmake_minimum_required(VERSION 3.15...3.26)

if(APPLE AND EXISTS "/opt/homebrew/bin/g++-13")
    set(CMAKE_CXX_COMPILER "/opt/homebrew/bin/g++-13")
endif()

project(
    GAME_OF_LIFE
    VERSION 1.0
    LANGUAGES CXX
)

# Enable testing
enable_testing()


# Set C++ standard to C++11
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



# Add cmake modules to path so it can find them.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Include the cmake module to find Doxygen
include(Doxygen)
Doxygen(src docs)

# Include clang-tidy
#include(ClangFormat)

#Include clang-tidy
#include(ClangTidy)

# Include other CMake submodules in the source directory
add_subdirectory(src)

# Include the test directory
add_subdirectory(test)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)


# Find MPI
find_package(MPI REQUIRED)

# Find OpenMP
find_package(OpenMP REQUIRED)

if(OpenMP_CXX_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

if (TARGET game)
    # Link MPI to the executable
    target_link_libraries(game PUBLIC MPI::MPI_CXX)

    if (OpenMP_CXX_FOUND)
        # Link OpenMP to the executable
        target_link_libraries(game PUBLIC OpenMP::OpenMP_CXX)
    endif()
endif()




