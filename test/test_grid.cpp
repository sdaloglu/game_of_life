#include <gtest/gtest.h>
#include "../src/include/grid.h"
#include <mpi.h>

/**
 * @brief Test for the constructor of the Grid class which internally calls initializeGrid function.
 * 
 */
TEST(GridTests, InitializesCorrectly) {

    // Set the size of the grid to a square grid of size 10x10
    int size1 = 5;
    int size2 = 5;

    // Set the seed for robustness of the test
    int seed = 42;

    // The expected grid of size 5x5 with random seed 42 was found by manually initializing the grid and printing
    int expected_grid[25] = {0, 1, 1, 1, 0,
                            1, 1, 0, 0, 1,
                            0, 1, 0, 1, 0,
                            0, 0, 0, 0, 0,
                            1, 0, 0, 1, 1};


    Grid grid_actual(size1, size2, seed);
    int* grid_ptr = grid_actual.getGrid();
    for (int i = 0; i < size1*size2; i++){
        bool isCorrect = grid_ptr[i] == expected_grid[i];
        ASSERT_TRUE(isCorrect)<< "Mismatch at row " << i/size1 << " and column " << i%size2;

    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Test for the indexing operator of the Grid class.
 * 
 */
TEST(GridTests, IndexOperator){
    // Set the size of the grid to a square grid of size 10x10
    int size1 = 5;
    int size2 = 5;

    // Set the seed for robustness of the test
    int seed = 42;

    // The expected grid of size 5x5 with random seed 42 was found by manually initializing the grid and printing
    int expected_grid[25] = {0, 1, 1, 1, 0,
                            1, 1, 0, 0, 1,
                            0, 1, 0, 1, 0,
                            0, 0, 0, 0, 0,
                            1, 0, 0, 1, 1};

    Grid grid_actual(size1, size2, seed);
    int* grid_ptr = grid_actual.getGrid();
    for (int i = 0; i < size1; i++){
        for (int j =0; j < size2; j++){
            bool isCorrect = grid_actual(i, j) == expected_grid[i*size2+j];
            ASSERT_TRUE(isCorrect)<< "Mismatch at row " << i << " and column " << j;
        }  
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

TEST(GridTests, UpdateGridCorrectly) {

    // Checking Still Lifes
    int block[25] = {0, 0, 0, 0, 0,    // This is an example of a still life called Block
                    0, 0, 1, 1, 0,
                    0, 0, 1, 1, 0,
                    0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0};
    
    Grid grid_block(5, 5, 42);
    grid_block.setGrid(block);
    for (int time=0; time < 30; time++){
        grid_block.updateGrid();  // Update the grid 30 times
    }
    int* block_ptr = grid_block.getGrid();
    for (int i = 0; i < 25; i++){
        bool isCorrect = block[i] == block_ptr[i];
        ASSERT_TRUE(isCorrect)<< "Mismatch for 'Block' still grid at row " << i/5 << " and column " << i%5;

    }


    int beehive[25] = {0, 0, 0, 0, 0,    // This is an example of a still life called Beehive
                        0, 0, 1, 1, 0,
                        0, 1, 0, 0, 1,
                        0, 0, 1, 1, 0,
                        0, 0, 0, 0, 0};
    
    Grid grid_beehive(5, 5, 42);
    grid_beehive.setGrid(beehive);
    for (int time=0; time < 30; time++){
        grid_beehive.updateGrid();  // Update the grid 30 times
    }
    int* beehive_ptr = grid_beehive.getGrid();
    for (int i = 0; i < 25; i++){
        bool isCorrect = beehive[i] == beehive_ptr[i];
        ASSERT_TRUE(isCorrect)<< "Mismatch for 'Beehive' still grid at row " << i/5 << " and column " << i%5;

    }

    int loaf[36] = {0, 0, 0, 0, 0, 0,    // This is an example of a still life called Loaf
                    0, 0, 1, 1, 0, 0,
                    0, 1, 0, 0, 1, 0,
                    0, 0, 1, 0, 1, 0,
                    0, 0, 0, 1, 0, 0,
                    0, 0, 0, 0, 0, 0};

    Grid grid_loaf(6, 6, 42);
    grid_loaf.setGrid(loaf);
    for (int time=0; time < 30; time++){
        grid_loaf.updateGrid();  // Update the grid 30 times
    }
    int* loaf_ptr = grid_loaf.getGrid();
    for (int i = 0; i < 36; i++){
        bool isCorrect = loaf[i] == loaf_ptr[i];
        ASSERT_TRUE(isCorrect)<< "Mismatch for 'Loaf' still grid at row " << i/6 << " and column " << i%6;

    }
    
    int boat[25] = {0, 0, 0, 0, 0,    // This is an example of a still life called Boat
                    0, 1, 1, 0, 0,
                    0, 1, 0, 1, 0,
                    0, 0, 1, 0, 0,
                    0, 0, 0, 0, 0};

    Grid grid_boat(5, 5, 42);
    grid_boat.setGrid(boat);
    for (int time=0; time < 30; time++){
        grid_boat.updateGrid();  // Update the grid 30 times
    }
    int* boat_ptr = grid_boat.getGrid();
    for (int i = 0; i < 25; i++){
        bool isCorrect = boat[i] == boat_ptr[i];
        ASSERT_TRUE(isCorrect)<< "Mismatch for 'Boat' still grid at row " << i/5 << " and column " << i%5;
    }

    int tub[25] = {0, 0, 0, 0, 0,    // This is an example of a still life called Tub
                    0, 0, 1, 0, 0,
                    0, 1, 0, 1, 0,
                    0, 0, 1, 0, 0,
                    0, 0, 0, 0, 0};

    Grid grid_tub(5, 5, 42);
    grid_tub.setGrid(tub);
    for (int time=0; time < 30; time++){
        grid_tub.updateGrid();  // Update the grid 30 times
    }
    int* tub_ptr = grid_tub.getGrid();
    for (int i = 0; i < 25; i++){
        bool isCorrect = tub[i] == tub_ptr[i];
        ASSERT_TRUE(isCorrect)<< "Mismatch for 'Tub' still grid at row " << i/5 << " and column " << i%5;
       
    }


    // Checking Oscillators

    int blinker1[25] = {0, 0, 0, 0,    // This is an example of an oscillator called Blinker
                        0, 1, 0, 0,
                        0, 1, 0, 0,
                        0, 1, 0, 0,
                        0, 0, 0, 0};
    int blinker2[25] = {0, 0, 0, 0,    // This is what the grid should look like after odd number of updates
                        0, 0, 0, 0,
                        1, 1, 1, 0,
                        0, 0, 0, 0,
                        0, 0, 0, 0};
    Grid grid_blinker(5, 4, 42);
    grid_blinker.setGrid(blinker1);
    for (int time=0; time < 15; time++){
        grid_blinker.updateGrid();  // Update the grid 30 times
    }
    int* blinker_ptr = grid_blinker.getGrid();
    for (int i = 0; i < 20; i++){
        bool isCorrect = blinker2[i] == blinker_ptr[i];
        ASSERT_TRUE(isCorrect)<< "Mismatch for 'Blinker' oscillator grid at row " << i/5 << " and column " << i%4;
        
    }

    // Update the blinker one more time to check if it returns to the original state
    grid_blinker.updateGrid();
    int* blinker_ptr2 = grid_blinker.getGrid();
    for (int i = 0; i < 20; i++){
        bool isCorrect = blinker1[i] == blinker_ptr2[i];
        ASSERT_TRUE(isCorrect)<< "Mismatch for 'Blinker' oscillator grid at row " << i/5 << " and column " << i%4;
       
    }


    // Checking Spaceships
    

}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


TEST(GridTests, PrintGrid){

    // Example grid to be printed
    int actual_grid[25] = {0, 1, 1, 1, 0,
                            1, 1, 0, 0, 1,
                            0, 1, 0, 1, 0,
                            0, 0, 0, 0, 0,
                            1, 0, 0, 1, 1};
    
    // Initialize the grid with the example grid
    Grid grid_actual(5, 5, 42);
    grid_actual.setGrid(actual_grid);

    testing::internal::CaptureStdout();  // Using GoogleTest's function to capture the output to the console instead of printing
    grid_actual.printGrid();    
    std::string output = testing::internal::GetCapturedStdout();  // Stored output of the printGrid function

    // Alive cells (1) are represented by 'o' and dead cells (0) are represented by '.'
    std::string expected_output = ". o o o . \no o . . o \n. o . o . \n. . . . . \no . . o o \n";
    
    bool isCorrect = output == expected_output;
    ASSERT_TRUE(isCorrect)<< "Mismatch in the output of the printGrid function";
 
}



// Add tests for MPI functions here --> communicateBoundary and reorganizeGrid