# Global CMakeLists.txt file for the GAME_OF_LIFE project

cmake_minimum_required(VERSION 3.15...3.26)

project(
    GAME_OF_LIFE
    VERSION 1.0
    LANGUAGES CXX
)


# Set C++ standard to C++11
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



# Add cmake modules to path so it can find them.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Include the cmake module to find Doxygen
include(Doxygen)
Doxygen(src docs)

# Include clang-tidy
include(ClangFormat)

#Include clang-tidy
include(ClangTidy)

# Include other CMake submodules in the source directory
add_subdirectory(src)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)


# Find MPI
find_package(MPI REQUIRED)
# Link MPI to the executable
target_link_libraries(game PUBLIC MPI::MPI_CXX)


