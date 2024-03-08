#ifndef GRID_H    // include guard
#define GRID_H

#include <vector>
#include <string>
#include <mpi.h>

// Declaring a class for initializing the grid and updating the grid
class Grid {    
public:

    // Declaring a constructor for the grid
    Grid(int size1, int size2, int seed);

    //Declaring a destructor for the grid
    ~Grid();

    //Alternatively user can provide an initial grid
    void setGrid(const int* initialGrid);  // User input is a 2D array

    // Updating function for the grid
    void updateGrid();
    
    // Printing function for the grid
    void printGrid();

    // Define an indexing function to access the elements of the grid (by using 2D index convention) 
    int& operator()(int row, int col);

    // Define a function to communicate the boundary cells with the neighboring processes
    void communicateBoundaryCells(int rank, int nranks);


private:

    // Declaring the size of the grid as a private variable
    int size1, size2;

    // Declaring the grid as a private variable
    int* grid;  // Pointer to a pointer (1D array to represent a 2D array -->by manipulating indices)

    // Private helper function to count the number of live neighbors inside the updateGrid function
    int countLiveNeighbors(int row, int col);

    // Private initializing function for the grid to be called from the constructor
    void initializeGrid(int seed);

    // MPI related objects
    int rank, nranks;
    MPI_Comm comm = MPI_COMM_WORLD;
    

};




#endif // GRID_H

