#ifndef GRID_H // include guard
#define GRID_H

#include <mpi.h>

#include <string>
#include <vector>

// Declaring a class for initializing the grid and updating the grid
class Grid {
public:
  // Declaring a default constructor for the grid
  Grid();

  // Declaring a constructor for the grid
  Grid(int size1, int size2, int seed);

  // Declaring a destructor for the grid
  ~Grid();

  // Define a function to get the pointer to the grid
  int *getGrid() const;

  // Declaring a copy constructor for the grid
  Grid &operator=(const Grid &other);

  // Define an indexing function to access the elements of the grid (by using 2D
  // index convention)
  int &operator()(int row, int col);

  // Alternatively user can provide an initial grid
  void setGrid(const int *initialGrid); // User input is a 2D array

  // Function to get the size of the grid
  int getSize() const;

  // Updating function for the grid by using classical counting of live
  void updateGrid();

  // Printing function for the grid
  void printGrid();

  // Define a function to reorganize 1D grid so that each chunk to be scattered
  // is contiguous in memory
  void reorganizeGrid(int n_process_x, int n_process_y, int grid_size);

  // Reversing the reorganization of the grid
  void inverseReorganizeGrid(int n_process_x, int n_process_y, int grid_size);

  // vertical padding function used in MPI
  void AddVerticalPadding();

  // vertical halo exchange function used in MPI
  void VerticalHaloExchange(int rank, int nranks, MPI_Comm comm);

  // Horizontal padding function used in MPI
  void AddHorizontalPadding();

  // horizontal halo exchange function used in MPI
  void HorizontalHaloExchange(int rank, int nranks, MPI_Comm comm);

  // vertical convolution function used in MPI
  void VerticalConv();

  // horizontal convolution function used in MPI
  void HorizontalConv();

  // Game of life rules
  void ApplyGameRules(const Grid &conv_grid);

private:
  // Declaring the size of the grid as a private variable
  int size1, size2;

  // Declaring the grid as a private variable
  int *grid; // Pointer to a pointer (1D array to represent a 2D array -->by
             // manipulating indices)

  // Private helper function to count the number of live neighbors inside the
  // updateGrid function
  int countLiveNeighbors(int row, int col);

  // Private initializing function for the grid to be called from the
  // constructor
  void initializeGrid(int seed);

  // MPI related objects
  // int rank, nranks;
  // MPI_Comm comm = MPI_COMM_WORLD;
};

#endif // GRID_H
