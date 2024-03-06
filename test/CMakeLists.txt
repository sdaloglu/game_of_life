add_executable(test 
    test1.cpp 
    test2.cpp
   
)

target_include_directories(test
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)