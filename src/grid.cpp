/*
    Contains the implementation of the Grid class
    The functions defined are:

    initializeGrid: Initializes the grid with random binary values
    updateGrid: Updates the grid based on the rules of the game
    printGrid: Prints the grid to the console



*/

#include "grid.h"
#include <iostream>  // for input and output
#include <vector>  // for vector
#include <cstdlib>  // for rand and srand, to generate random numbers


//*****************************************************************************************
//***************************----PUBLIC----************************************************
//*****************************************************************************************



//******************************************************************************
// Constructor for the grid
//******************************************************************************
Grid::Grid(int size, int seed) : size(size), grid(size, std::vector<int>(size)) {
    /*
        Constructor for the grid to initialize the class's member variables

        Args:
            size: int, the size of the grid
            seed: int, the seed for the random number generator, 
                this ensures robustness of the simulation and analysis

        Returns:
            void
    */
    initializeGrid(seed);    // Populate the grid with random binary values
}


//******************************************************************************
// User input grid initialization
//******************************************************************************
void Grid::setGrid(const std::vector<std::vector<int>>& initialGrid){
    /*
        Initializes the grid with the user input, assume the input grid is a square

        Args:
            initialGrid: std::vector<std::vector<int>>, the initial grid provided by the user

        Returns:
            void
    */

    size = initialGrid.size();  // size of the grid
    grid = initialGrid;  // initialize the grid
}


//******************************************************************************
// defining a function to update the grid
//******************************************************************************
void Grid::updateGrid(){

    /*
        Updates the grid based on the rules of the game:
            - 1. Any live cell with fewer than two live neighbors dies, as if by underpopulation
            - 2. Any live cell with more than three live neighbors dies, as if by overpopulation
            - 3. Any live cell with two or three live neighbors lives on to the next generation
            - 4. Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction
    
    */

    // Create a new grid to store the updated values after applying the rules of the game
    std::vector<std::vector<int>> new_grid(size, std::vector<int>(size,0));

    // Iterate through each cell in the grid
    for (int row=0; row<size;++row){    // Iterating over rows
        for (int col=0; col<size;++col){    // Iterating over columns
            // Counting the number of live neighbors
        
            int live_neighbors = countLiveNeighbors(row, col);
            // The number of alive neighbors has been counted, now apply the rules of the game

            //Rule number1: Under-population
            if (grid[row][col]==1 && live_neighbors<2){
                new_grid[row][col] = 0; // the current cell dies
            }
            //Rule number2: Over-population
            else if (grid[row][col]==1 && live_neighbors>3){
                new_grid[row][col] = 0; // the current cell dies
            }
            //Rule number3: Survival - assuming its alive already
            else if((live_neighbors==2 || live_neighbors==3) && grid[row][col]==1){
                new_grid[row][col] = 1; // the current alive cell continues to live on to the next generation 
            }

            //Rule number4: Reproduction
            else if(live_neighbors==3 && grid[row][col]==0){
                new_grid[row][col] = 1; // the current dead cell becomes alive in the next generation
            }
            else{
                new_grid[row][col] = 0; // the current dead cell remains dead

            }
        }
    }

    grid = new_grid;
}


//******************************************************************************
// defining a function to print the grid
//******************************************************************************
void Grid::printGrid(){
    /*
        Prints the grid to the terminal

        Args:
            grid: std::vector<std::vector<int>>, the grid to be printed

        Returns:
            void

    
    */

    // Iterate through the grid and print the values to the terminal
    for (int row = 0; row < size; ++row){
        for (int col = 0; col < size; ++col){

            // Use of ternary operator to print the values

            std::cout << (grid[row][col] ? "o" : ".") << " ";
        }
        std::cout << std::endl;  // End of each row
    }

}



//*****************************************************************************************
//***************************----PRIVATE----***********************************************
//*****************************************************************************************


//******************************************************************************
// define a function for counting the number of live neighbors
//******************************************************************************
int Grid::countLiveNeighbors(int row, int col){

    /*
        Counts the number of live neighbors for each cell in the grid

        Args:
            row: int, the row index of the cell
            col: int, the column index of the cell

        Returns:
            int, the number of live neighbors for each cell in the grid
    */

    // Initialize the number of live neighbors
    int live_neighbors = 0;

    // Iterate through the neighbour of the current cell - nested loop
    for (int x=-1; x<=1; ++x){
        for (int y=-1; y<=1; ++y){
        
            // This mini 3x3 grid also includes the current cell which we want to skip
            if (x==0 && y==0){continue;}

            // Assuming boundaries are periodic, calculating the coordinates of the neighbors
            int x_neighbour = ((row + x) + size) % size;    // Adding and taking modulo of size ensures periodic boundaries
            int y_neighbour = ((col + y) + size) % size;    // Assuming the grid is a square --> size here is the same as the total number of columns


            // For periodic boundaries, no need to check if any of the neighbors are outside the grid

            // Neighbour pixel is inside the grid, count the number of live neighbors
            live_neighbors += grid[x_neighbour][y_neighbour];
                    
        }
    }

    return live_neighbors;


}


//******************************************************************************
// Defining a function to initialize the grid
//******************************************************************************
void Grid::initializeGrid(int seed){
    /*
        Initializes the grid with random binary values

        Args:
            size: int, the size of the grid
            seed: int, the seed for the random number generator, 
                this ensures robustness of the simulation and analysis

        Returns:
            std::vector<std::vector<int> >, the initialized grid
    */

    // Setting the seed for the random number generator
    srand(seed);

    // Fill the grid with random binary values
    for (int i = 0; i < size; ++i){
        for (int j = 0; j < size; ++j){
            grid[i][j] = rand() % 2; // modulo 2 ensures that the value is either 0 or 1 (even or odd)
        }
    }
}

