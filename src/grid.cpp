/**
 * @file grid.cpp
 * @brief Contains the implementation of the Grid class
 * The functions defined are:
 * - initializeGrid: Initializes the grid with random binary values
 * - updateGrid: Updates the grid based on the rules of the game
 * - printGrid: Prints the grid to the console
 * - countLiveNeighbors: Counts the number of live neighbors for each cell in
 * the grid
 * - reorganizeGrid: Reorganizes the 1D grid so that each chunk to be scattered
 * is contiguous in memory
 * - communicateBoundary: Communicates the boundary cells with the neighboring
 * processes
 * - getGrid: Returns the pointer to the grid
 * - setGrid: Initializes the grid with the user input
 * - operator(): Access the elements of the grid (by using 2D index convention)
 * - Grid: Constructor for the grid to initialize the class's member variables
 * - ~Grid: Destructor for the grid to deallocate the memory used by the grid
 * object
 */
#include "grid.h"

#include <mpi.h>  // for MPI functions

#include <cstdlib>   // for rand and srand, to generate random numbers
#include <iostream>  // for input and output
#include <vector>    // for vector

//*****************************************************************************************
//***************************----PUBLIC----************************************************
//*****************************************************************************************

/**
 * @brief Constructor for the grid to initialize the class's member variables.
 *
 * @param size1 int, the size of the grid
 * @param size2 int, the size of the grid
 * @param seed int, the seed for the random number generator, this ensures
 * robustness of the simulation and analysis
 */
Grid::Grid(int size1, int size2, int seed) : size1(size1), size2(size2) {
  grid = new int[size1 * size2];  // Allocate memory for the grid

  initializeGrid(seed);  // Populate the grid with random binary values
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief  Destructor for the grid to deallocate the memory used by the grid
 * object.
 *
 */
Grid::~Grid() {
  delete[] grid;  // Deallocate memory for the grid
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Returns the pointer to the grid
 *
 * @return int* The pointer to the grid
 */
int* Grid::getGrid() { return grid; };

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Access the elements of the grid using 2D index convention
 *
 * @param row int, the row index of the cell
 * @param col int, the column index of the cell
 * @return int&, the value of the cell in the grid
 */
int& Grid::operator()(int row, int col) { return grid[row * size2 + col]; };

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Initializes the grid with the user input, assumes the input grid is a
 * square
 *
 * @param initialGrid int*, the initial grid provided by the user
 */
void Grid::setGrid(const int* initialGrid) {
  memcpy(
      grid, initialGrid,
      sizeof(int) * size1 * size2);  // Memory copy the user input to the grid
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Updates the grid based on the rules of the game:
 * - 1. Any live cell with fewer than two live neighbors dies, as if by
 * underpopulation
 * - 2. Any live cell with more than three live neighbors dies, as if by
 * overpopulation
 * - 3. Any live cell with two or three live neighbors lives on to the next
 * generation
 * - 4. Any dead cell with exactly three live neighbors becomes a live cell, as
 * if by reproduction
 */
void Grid::updateGrid() {
  // Create a new grid to store the updated values after applying the rules of
  // the game
  int* new_grid = new int[size1 * size2];  // Allocate memory for the new grid

  // Iterate through each cell in the grid
  for (int i = 0; i < size1; ++i) {  // Iterating over rows

    for (int j = 0; j < size2; ++j) {  // Iterating over columns

      // Counting the number of live neighbors
      int live_neighbors = countLiveNeighbors(i, j, "1D_convolution");
      // The number of alive neighbors has been counted, now apply the rules of
      // the game

      // Create an index that uses the 2D index convention in accessing 1D array
      int index = i * size2 + j;

      // Additionally one can use "*this" instead of "grid" to use the
      // operator() for indexing However, new_grid local variable is not a
      // member of the class, hence requires "index"

      // Rule number1: Under-population
      if (grid[index] == 1 && live_neighbors < 2) {
        new_grid[index] = 0;  // the current cell dies
      }
      // Rule number2: Over-population
      else if (grid[index] == 1 && live_neighbors > 3) {
        new_grid[index] = 0;  // the current cell dies
      }
      // Rule number3: Survival - assuming its alive already
      else if ((live_neighbors == 2 || live_neighbors == 3) &&
               grid[index] == 1) {
        new_grid[index] = 1;  // the current alive cell continues to live on to
                              // the next generation
      }

      // Rule number4: Reproduction
      else if (live_neighbors == 3 && grid[index] == 0) {
        new_grid[index] =
            1;  // the current dead cell becomes alive in the next generation
      } else {
        new_grid[index] = 0;  // the current dead cell remains dead
      }
    }
  }

  // Deallocate the memory used by the previous grid
  delete[] grid;  // Deallocate old "grid"

  grid = new_grid;  // Set the grid to the new grid
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Prints the grid to the terminal
 *
 */
void Grid::printGrid() {
  // Iterate through the grid and print the values to the terminal
  for (int row = 0; row < size1; ++row) {
    for (int col = 0; col < size2; ++col) {
      // Use of ternary operator to print the values

      std::cout
          << ((*this)(row, col) ? "o" : ".")
          << " ";  // "*this" here is equivalent to "grid" in the main function
    }
    std::cout << std::endl;  // End of each row
  }
};

//*****************************************************************************************
//***************************----PRIVATE----***********************************************
//*****************************************************************************************

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Counts the number of live neighbors for each cell in the grid
 *
 * @param row int, the row index of the cell
 * @param col int, the column index of the cell
 * @param method std::string, the method to use for counting the number of live
 * neighbors
 * @return int the number of live neighbors for each cell in the grid
 */
int Grid::countLiveNeighbors(int row, int col,
                             std::string method = "for_loop") {
  // Initialize the number of live neighbors
  int live_neighbors = 0;

  if (method == "for_loop") {
    // Iterate through the neighbour of the current cell - nested loop
    for (int dx = -1; dx <= 1; ++dx) {
      for (int dy = -1; dy <= 1; ++dy) {
        // This mini 3x3 grid also includes the current cell which we want to
        // skip
        if (dx == 0 && dy == 0) {
          continue;
        }

        // Assuming boundaries are periodic, calculating the coordinates of the
        // neighbors
        int x_neighbour =
            ((row + dx) + size1) % size1;  // Adding and taking modulo of size
                                           // ensures periodic boundaries
        int y_neighbour =
            ((col + dy) + size2) %
            size2;  // Assuming the grid is a square --> size1 = size2

        // For periodic boundaries, no need to check if any of the neighbors are
        // outside the grid

        // Neighbour pixel is inside the grid, count the number of live
        // neighbors
        live_neighbors +=
            (*this)(x_neighbour, y_neighbour);  // "*this" here is equivalent to
                                                // "grid" in the main function
      }
    }
  }

  else if (method == "1D_convolution") {
    // Instead of defining a kernel we can treat the nested for loop index as a
    // 2D kernel This nested for-loop can be reduced to a single for-loop

    // Indices for navigating through 8 neighbors
    int dx[8] = {-1, -1, -1, 0, 0,
                 1,  1,  1};  // Notice the central index is removed since it is
                              // the current cell
    int dy[8] = {-1, 0, 1, -1, 1,
                 -1, 0, 1};  // Notice the central index is removed since it is
                             // the current cell

    // By removing the central index, we also remove the need for the if
    // statement to skip the current cell

    // Iterate through the 8 neighbors of the current cell - single loop
    for (int i = 0; i < 8; ++i) {
      // Assuming boundaries are periodic, calculating the coordinates of the
      // neighbors
      int x_neighbour =
          ((row + dx[i]) + size1) % size1;  // Adding and taking modulo of size
                                            // ensures periodic boundaries
      int y_neighbour = ((col + dy[i]) + size2) % size2;

      live_neighbors +=
          (*this)(x_neighbour, y_neighbour);  // "*this" here is equivalent to
                                              // "grid" in the main function
    }
  }

  return live_neighbors;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Initializes the grid with random binary values
 *
 * @param seed int, the seed for the random number generator, this ensures
 * robustness of the simulation and analysis
 */
void Grid::initializeGrid(int seed) {
  // Setting the seed for the random number generator
  srand(seed);

  // Fill the grid with random binary values
  for (int i = 0; i < size1; ++i) {
    for (int j = 0; j < size2; ++j) {
      (*this)(i, j) =
          rand() %
          2;  // modulo 2 ensures that the value is either 0 or 1 (even or odd)
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief MPI function to reorganize the 1D grid so that each chunk to be
 * scattered is contiguous in memory
 *
 * @param nranks int, the number of processes
 */
void Grid::reorganizeGrid(int nranks) {
  // Process grid size
  int process_grid_size =
      sqrt(nranks);  // Assuming the number of processes is a perfect square

  int* new_grid = new int[size1 * size2];  // Allocate memory for the new grid
  int num_rows_subgrid =
      size1 / process_grid_size;  // Number of rows in each subgrid
  int num_cols_subgrid =
      size2 / process_grid_size;  // Number of columns in each subgrid

  // Reorganize the grid so that each chunk to be scattered is contiguous in
  // memory

  for (int subgrid_row = 0; subgrid_row < num_rows_subgrid; ++subgrid_row) {
    for (int subgrid_col = 0; subgrid_col < num_cols_subgrid; ++subgrid_col) {
      for (int row = 0; row < process_grid_size; ++row) {
        for (int col = 0; col < process_grid_size; ++col) {
          // Calculate the original row and column of the cell
          int original_row = subgrid_row * process_grid_size + row;
          int original_col = subgrid_col * process_grid_size + col;
          // Calculate the new index of the cell in the new grid
          int new_index =
              (subgrid_row * num_cols_subgrid * process_grid_size *
               process_grid_size) +
              (subgrid_col * process_grid_size * process_grid_size) +
              (row * process_grid_size) + col;
          // Calculate the original index of the cell in the old grid
          int original_index = original_row * size2 + original_col;
          new_grid[new_index] = grid[original_index];  // Copy the value of the
                                                       // cell to the new grid
        }
      }
    }
  }
  // Delete the old grid memory
  delete[] grid;

  // Set the grid to the new grid
  grid = new_grid;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief MPI function to communicate the boundary cells with the neighboring
 * processes
 *
 * @param rank int, the rank of the current process
 * @param nranks int, the number of processes
 */
void Grid::communicateBoundary(int rank, int nranks) {
  // 2D Domain decomposition has 8 neighbors
  // Calculate the rank of the neighboring processes

  int process_grid_size =
      sqrt(nranks);  // Assuming the number of processes is a perfect square
  int row =
      rank / process_grid_size;  // Calculate the row of the current process
  int col =
      rank % process_grid_size;  // Calculate the column of the current process

  // Define the ranks of the neighboring processes
  int top =
      (row - 1 + process_grid_size) % process_grid_size * process_grid_size +
      col;
  int bottom = (row + 1) % process_grid_size * process_grid_size + col;
  int left = row * process_grid_size +
             (col - 1 + process_grid_size) % process_grid_size;
  int right = row * process_grid_size + (col + 1) % process_grid_size;
  int top_left =
      (row - 1 + process_grid_size) % process_grid_size * process_grid_size +
      (col - 1 + process_grid_size) % process_grid_size;
  int top_right =
      (row - 1 + process_grid_size) % process_grid_size * process_grid_size +
      (col + 1) % process_grid_size;
  int bottom_left = (row + 1) % process_grid_size * process_grid_size +
                    (col - 1 + process_grid_size) % process_grid_size;
  int bottom_right = (row + 1) % process_grid_size * process_grid_size +
                     (col + 1) % process_grid_size;

  // Communicate the boundary cells with the neighboring processes
  // Send the top row to the top neighbor and receive the bottom row from the
  // top neighbor
  MPI_Sendrecv(&grid[size2], size2, MPI_INT, top, 0, &grid[(size1 - 1) * size2],
               size2, MPI_INT, bottom, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  // Send the bottom row to the bottom neighbor and receive the top row from the
  // bottom neighbor
  MPI_Sendrecv(&grid[(size1 - 2) * size2], size2, MPI_INT, bottom, 0, &grid[0],
               size2, MPI_INT, top, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  // Send the left column to the left neighbor and receive the right column from
  // the left neighbor
  for (int i = 0; i < size1; ++i) {
    MPI_Sendrecv(&grid[i * size2 + 1], 1, MPI_INT, left, 0,
                 &grid[i * size2 + size2 - 1], 1, MPI_INT, right, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  // Send the right column to the right neighbor and receive the left column
  // from the right neighbor
  for (int i = 0; i < size1; ++i) {
    MPI_Sendrecv(&grid[i * size2 + size2 - 2], 1, MPI_INT, right, 0,
                 &grid[i * size2], 1, MPI_INT, left, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
  }

  // Communicate the corner cells with the neighboring processes
  MPI_Sendrecv(&grid[1], 1, MPI_INT, top_left, 0, &grid[size2 * size1 - 1], 1,
               MPI_INT, bottom_right, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  MPI_Sendrecv(&grid[size2 - 2], 1, MPI_INT, top_right, 0,
               &grid[size2 * size1 - size2], 1, MPI_INT, bottom_left, 0,
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  MPI_Sendrecv(&grid[size2 * size1 - size2 + 1], 1, MPI_INT, bottom_left, 0,
               &grid[size2 - 1], 1, MPI_INT, top_right, 0, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);

  MPI_Sendrecv(&grid[size2 * size1 - 2], 1, MPI_INT, bottom_right, 0,
               &grid[size2], 1, MPI_INT, top_left, 0, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);

  MPI_Barrier(MPI_COMM_WORLD);  // Synchronize all processes
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
