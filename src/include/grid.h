#ifndef GRID_H    // include guard
#define GRID_H

#include <vector>
#include <string>

// Declaring a class for initializing the grid and updating the grid
class Grid {    
public:

    // Declaring a constructor for the grid
    Grid(int size, int seed);


    // Static means that the function can be called without an instance of the class

    //Alternatively user can provide an initial grid
    void setGrid(const std::vector<std::vector<int>>& initialGrid);

    // Updating function for the grid
    void updateGrid();
    
    // Printing function for the grid
    void printGrid();


private:

    // Declaring the size of the grid
    // Private to make sure it is not modified outside the class
    int size;

    // Declaring the grid as a private variable
    std::vector<std::vector<int>> grid;

    // Private helper function to count the number of live neighbors inside the updateGrid function
    int countLiveNeighbors(int row, int col);

    // Private initializing function for the grid to be called from the constructor
    void initializeGrid(int seed);
    

};



#endif // GRID_H
