cmake_minimum_required(VERSION 3.15...3.26)

project(
    GAME_OF_LIFE
    VERSION 1.0
    LANGUAGES CXX
)


# Set C++ standard to C++11
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)  

# Find MPI
find_package(MPI REQUIRED)
# Link MPI to the executable
target_link_libraries(game PUBLIC MPI::MPI_CXX)


# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
# Link SFML to the executable
target_link_libraries(game PUBLIC sfml-graphics sfml-window sfml-system)

# Optional: add cmake modules to path so it can find them.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)


