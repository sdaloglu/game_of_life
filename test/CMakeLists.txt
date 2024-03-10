include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.13.0
)

set(gtest\_force\_shared\_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


add_executable(test_grid test_grid.cpp)
target_link_libraries(test_grid 
            PRIVATE grid
            GTest: gtest_main)


include(GoogleTest)
gtest_discover_tests(test_grid)