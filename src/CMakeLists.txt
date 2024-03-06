add_subdirectory(timing)

add_executable(game 
    main.cpp 
    grid.cpp
   
)

target_include_directories(game
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(game PUBLIC timing_lib)