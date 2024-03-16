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

#include <mpi.h> // for MPI functions

#include <cstdlib>  // for rand and srand, to generate random numbers
#include <iostream> // for input and output
#include <vector>   // for vector

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
  grid = new int[size1 * size2]; // Allocate memory for the grid

  initializeGrid(seed); // Populate the grid with random binary values
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief  Destructor for the grid to deallocate the memory used by the grid
 * object.
 *
 */
Grid::~Grid() {
  delete[] grid; // Deallocate memory for the grid
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Returns the pointer to the grid
 *
 * @return int* The pointer to the grid
 */
int *Grid::getGrid() const { return grid; };

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Access the elements of the grid using 2D index convention
 *
 * @param row int, the row index of the cell
 * @param col int, the column index of the cell
 * @return int&, the value of the cell in the grid
 */
int &Grid::operator()(int row, int col) { return grid[row * size2 + col]; };

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Initializes the grid with the user input, assumes the input grid is a
 * square
 *
 * @param initialGrid int*, the initial grid provided by the user
 */
void Grid::setGrid(const int *initialGrid) {
  memcpy(
      grid, initialGrid,
      sizeof(int) * size1 * size2); // Memory copy the user input to the grid
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


int Grid::getSize() const { return size1; }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Updates the grid by using classical counting method and based on the
 * rules of the game:
 * - 1. Any live cell with fewer than two live neighbors dies, as if by
 * underpopulation
 * - 2. Any live cell with more than three live neighbors dies, as if by
 * overpopulation
 * - 3. Any live cell with two or three live neighbors lives on to the next
 * generation
 * - 4. Any dead cell with exactly three live neighbors becomes a live cell, as
 * if by reproduction
 */
void Grid::updateGridCounting() {
  // Create a new grid to store the updated values after applying the rules of
  // the game
  int *new_grid = new int[size1 * size2]; // Allocate memory for the new grid

  // Iterate through each cell in the grid
  for (int i = 0; i < size1; ++i) { // Iterating over rows

    for (int j = 0; j < size2; ++j) { // Iterating over columns

      // Counting the number of live neighbors
      int live_neighbors = countLiveNeighbors(i, j);
      // The number of alive neighbors has been counted, now apply the rules of
      // the game

      // Create an index that uses the 2D index convention in accessing 1D array
      int index = i * size2 + j;

      // Additionally one can use "*this" instead of "grid" to use the
      // operator() for indexing However, new_grid local variable is not a
      // member of the class, hence requires "index"

      bool alive = grid[index] == 1;
      bool survive = alive && (live_neighbors == 2 ||
                               live_neighbors == 3); // Rule number3: Survival
      bool reproduce =
          !alive && live_neighbors == 3; // Rule number4: Reproduction
      new_grid[index] =
          survive || reproduce
              ? 1
              : 0; // If rule 3 or 4 is satisfied, the cell is alive, otherwise
                   // it is dead due to rule 1 or 2
    }
  }

  // Deallocate the memory used by the previous grid
  // delete[] grid;  // Deallocate old "grid"
  // grid = new_grid;  // Set the grid to the new grid

  // Alternatively swap old grid with new grid without deallocating memory
  // This is more efficient than deallocating and allocating memory
  std::swap(grid, new_grid);

  // Deallocate the memory used by the previous grid
  delete[] new_grid; // After the swap, new_grid is the old grid
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Grid::padding(int new_size1, int new_size2) {
  // Takes the grid and adds a periodic padding if necessary to make the grid
  // size divisible by the number of processes in each direction

  int *new_grid =
      new int[new_size1 * new_size2]; // Allocate memory for the new grid

  // Fill the new grid with the old grid plus the periodic padding
  for (int i = 0; i < new_size1; ++i) {
    for (int j = 0; j < new_size2; ++j) {
      new_grid[i * new_size2 + j] = grid[(i % size1) * size2 + (j % size2)];
    }
  }

  // Delete the old grid memory
  delete[] grid;

  // Assign the new grid to the old grid
  grid = new_grid;

  // Update the size attributes of the grid
  size1 = new_size1;
  size2 = new_size2;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Grid::unpadGrid(int grid_size) {
  int *unpadded_grid = new int[grid_size * grid_size]; // Allocate memory for the new grid

  // Calculate the difference in size between the old and new grid
  int padding_x = (size2 - grid_size) / 2;
  int padding_y = (size1 - grid_size) / 2;

  // Fill the new grid with the old grid plus the periodic padding
  for (int i = 0; i < grid_size; ++i) {
    for (int j = 0; j < grid_size; ++j) {
      unpadded_grid[i * grid_size + j] = grid[i * size2 + j];
    }
  }

  // Delete the old grid memory
  delete[] grid;

  // Assign the new grid to the old grid
  grid = unpadded_grid;

  // Update the size attributes of the grid
  size1 = grid_size;
  size2 = grid_size;
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
          << " "; // "*this" here is equivalent to "grid" in the main function
    }
    std::cout << std::endl; // End of each row
  }
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief MPI function to reorganize the 1D grid so that each chunk to be
 * scattered is contiguous in memory
 *
 * @param nranks int, the number of processes
 */
void Grid::reorganizeGrid(int n_process_x, int n_process_y) {
  int *new_grid = new int[size1 * size2]; // Allocate memory for the new grid

  // Calculate the size of the each process's grid
  int process_cols =
      size2 / n_process_x; // Number of columns in each process's grid
  int process_rows =
      size1 / n_process_y; // Number of rows in each process's grid

  // Number of elements in each chunk to be scattered
  int chunk_size = process_cols * process_rows;

  // Reorganize the grid so that each chunk to be scattered is contiguous in
  // memory
  for (int proc_y = 0; proc_y < n_process_y;
       ++proc_y) { // Iterate through each row of processes
    for (int proc_x = 0; proc_x < n_process_x;
         ++proc_x) { // Iterate through each column of processes
      // Iterate through each cell in the process's grid
      for (int row = 0; row < process_rows; ++row) {
        for (int col = 0; col < process_cols; ++col) {
          int global_row = proc_y * process_rows + row;
          int global_col = proc_x * process_cols + col;
          int index_in_global = global_row * size2 + global_col;

          int index_in_chunk = row * process_cols + col;
          int chunk_start_index = (proc_y * n_process_x + proc_x) * chunk_size;

          new_grid[chunk_start_index + index_in_chunk] = grid[index_in_global];
        }
      }
    }
  }

  // Delete the old grid
  delete[] grid;

  // Swap the old grid with the new grid
  std::swap(grid, new_grid);
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Grid::inverseReorganizeGrid(int n_process_x, int n_process_y) {
  int *original_grid =
      new int[size1 * size2]; // Allocate memory for the new grid

  // Calculate the size of the each process's grid
  int process_cols =
      size2 / n_process_x; // Number of columns in each process's grid
  int process_rows =
      size1 / n_process_y; // Number of rows in each process's grid

  // Number of elements in each chunk to be scattered
  int chunk_size = process_cols * process_rows;

  // Iterate over the grid that is contiguous in memory for each process
  for (int proc_y = 0; proc_y < n_process_y;
       ++proc_y) { // Iterate through each row of processes
    for (int proc_x = 0; proc_x < n_process_x;
         ++proc_x) { // Iterate through each column of processes
      // Iterate through each cell in the process's grid
      for (int row = 0; row < process_rows; ++row) {
        for (int col = 0; col < process_cols; ++col) {
          int global_row = proc_y * process_rows + row;
          int global_col = proc_x * process_cols + col;
          int index_in_global = global_row * size2 + global_col;

          int index_in_chunk = row * process_cols + col;
          int chunk_start_index = (proc_y * n_process_x + proc_x) * chunk_size;

          original_grid[index_in_global] =
              grid[chunk_start_index + index_in_chunk];
        }
      }
    }
  }

  // Delete the old grid
  delete[] grid;

  // Swap the old grid with the new grid
  std::swap(grid, original_grid);
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Grid::AddVerticalPadding(int process_cols, int process_rows) {
  // Add vertical padding to the grid
  int *padded_grid = new int[(process_rows + 2) * process_cols]; // Allocate memory for the new grid

  // Initialize the top and bottom halo rows to 0
  std::fill_n(padded_grid, process_cols, 0);
  std::fill_n(&padded_grid[(process_rows + 1) * process_cols], process_cols, 0);

  // Fill the new grid with the old grid plus the vertical padding
  for (int i = 0; i < process_rows; ++i) {
    for (int j = 0; j < process_cols; ++j) {
      padded_grid[(i + 1) * process_cols + j] = grid[i * process_cols + j];
    }
  }

  // Delete the old grid
  delete[] grid;

  grid = padded_grid;

  // Update the size attributes of the grid
  size1 = process_rows + 2;
  size2 = process_cols;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Grid::VerticalHaloExchange(int rank, int ranks, MPI_Comm cart_comm) {

  // Determine the ranks of up and down neighbors
  int up, down;
  MPI_Cart_shift(cart_comm, 0, 1, &up, &down);

  MPI_Request send_request[2],
      recv_request[2]; // MPI request objects for non-blocking communication

  // Communicate the boundary cells with the neighboring processes
  // Send the top row to the top neighbor and receive the top row from the
  // bottom neighbor
  MPI_Isend(&grid[size2], size2, MPI_INT, up, 0, cart_comm,
            &send_request[0]);
  MPI_Irecv(&grid[(size1 - 1) * size2], size2, MPI_INT, down, 0, cart_comm,
            &recv_request[0]);

  // Send the bottom row to the bottom neighbor and receive the bottom row from
  // the top neighbor
  MPI_Isend(&grid[(size1 - 2) * size2], size2, MPI_INT, down, 0, cart_comm,
            &send_request[1]);
  MPI_Irecv(&grid[0], size2, MPI_INT, up, 0, cart_comm, &recv_request[1]);

  // Wait for the non-blocking communication to finish
  MPI_Waitall(2, send_request, MPI_STATUS_IGNORE);
  MPI_Waitall(2, recv_request, MPI_STATUS_IGNORE);
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Grid::AddHorizontalPadding(int process_cols, int process_rows) {
  // Add horizontal padding to the grid
  int *padded_grid = new int[process_rows * (process_cols + 2)]; // Allocate memory for the new grid

  // Initialize the left and right halo columns to 0
  for (int i = 0; i < process_rows; ++i) {
    padded_grid[i * (process_cols + 2)] = 0; // Leftmost column

    // Copy the original grid to the new grid
    for (int j = 0; j < process_cols; ++j) {
      padded_grid[i * (process_cols + 2) + (j + 1)] = grid[i * process_cols + j];
    }

    padded_grid[i * (process_cols + 2) + process_cols + 1] = 0; // Rightmost column
  }
  // Delete the old grid
  delete[] grid;

  grid = padded_grid;

  // Update the size attributes of the grid
  size1 = process_rows;
  size2 = process_cols + 2;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Grid::HorizontalHaloExchange(int rank, int ranks, MPI_Comm cart_comm) {

  // Determine the ranks of left and right neighbors
  int left, right;
  MPI_Cart_shift(cart_comm, 1, 1, &left, &right);

  // Accessing column is strided so we need to use MPI_Type_vector
  MPI_Datatype column_type;

  MPI_Type_vector(size1, 1, size2, MPI_INT, &column_type);
  MPI_Type_commit(&column_type);

  // Communicate the boundary cells with the neighboring processes
  MPI_Request send_request[2], recv_request[2]; // MPI request objects for non-blocking communication

  // Send the left column to the left neighbor and receive the left column from
  // the right neighbor
  MPI_Isend(&grid[1], 1, column_type, left, 0, cart_comm,
            &send_request[0]);
  MPI_Irecv(&grid[size2 - 1], 1, column_type, right, 0, cart_comm,
            &recv_request[0]);

  // Send the right column to the right neighbor and receive the right column
  // from the left neighbor
  MPI_Isend(&grid[size2 - 2], 1, column_type, right, 0, cart_comm,
            &send_request[1]);
  MPI_Irecv(&grid[0], 1, column_type, left, 0, cart_comm,
            &recv_request[1]);

  // Wait for the non-blocking communication to finish
  MPI_Waitall(2, send_request, MPI_STATUS_IGNORE);
  MPI_Waitall(2, recv_request, MPI_STATUS_IGNORE);

  // Free the MPI datatype
  MPI_Type_free(&column_type);
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Grid::VerticalConv(int process_cols, int process_rows) {
  // This function will be called on vertically padded grids
  // It will perform the vertical convolution that will shrink the padded grid
  // into the original size

  int *vertical_sum_array = new int[process_cols * process_rows]; // Allocate memory for the new grid

  for (int i = 0; i < process_rows; ++i) {
    for (int j = 0; j < process_cols; ++j) {
      int sum = 0;
      for (int dx = -1; dx <= 1; ++dx) {
        int x_neighbour = (i + dx) + 1; // adjust the index to account for the padding

        sum += grid[x_neighbour * process_cols + j];
      }
      vertical_sum_array[i * process_cols + j] = sum;
    }
  }

  // Free the original grid memory
  delete[] grid;

  grid = vertical_sum_array;

  // Update the vertically reduced size of the grid
  size1 = process_rows;
  size2 = process_cols;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Grid::HorizontalConv(int process_cols, int process_rows) {
  // This function will be called on horizontally padded grids
  // It will perform the horizontal convolution that will shrink the padded grid
  // into the original size

  int *horizontal_sum_array = new int[process_cols * process_rows]; // Allocate memory for the new grid

  // Convolution with no padding
  for (int i = 0; i < process_rows; ++i) {
    for (int j = 0; j < process_cols; ++j) {
      int sum = 0;
      for (int dy = -1; dy <= 1; ++dy) {
        int y_neighbour = (j + dy) + 1; // adjust the index to account for the padding

        sum += grid[i * (process_cols + 2) + y_neighbour];
      }
      horizontal_sum_array[i * process_cols + j] = sum;
    }
  }

  // Free the original grid memory
  delete[] grid;

  grid = horizontal_sum_array;

  // Update the horizontally reduced size of the grid
  size1 = process_rows;
  size2 = process_cols;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Grid::ApplyGameRules(const Grid &conv_grid) {

  // Updates the grid by applying Game of Life rules
  // The rules are updated to count for seperated 2D convolution
  // where the cell itself is also included in the convolution result

  const int *conv_grid_data = conv_grid.getGrid();
  // Iterate through each cell in the grid
  for (int i = 0; i < size1; ++i) { // Iterating over array
    for (int j = 0; j < size2; ++j) {
      int index = i * size2 + j;

      int live_neighbors = conv_grid_data[index]; // Use the convolution result at the index (includes cell itself)

      if (live_neighbors == 3) {
        grid[index] = 1; // Reproduction or survival if initially alive with 2 live neighbors
      } else if (live_neighbors == 4) {
        continue; // Survival for live cells with 3 live neighbors
      } else {
        grid[index] = 0; // Rule number1 and 2: Underpopulation and Overpopulation
      }
    }
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
int Grid::countLiveNeighbors(int row, int col) {
  // Initialize the number of live neighbors
  int live_neighbors = 0;

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
          ((row + dx) + size1) % size1; // Adding and taking modulo of size
                                        // ensures periodic boundaries
      int y_neighbour =
          ((col + dy) + size2) %
          size2; // Assuming the grid is a square --> size1 = size2

      // For periodic boundaries, no need to check if any of the neighbors are
      // outside the grid

      // Neighbour pixel is inside the grid, count the number of live
      // neighbors
      live_neighbors +=
          (*this)(x_neighbour, y_neighbour); // "*this" here is equivalent to
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
          2; // modulo 2 ensures that the value is either 0 or 1 (even or odd)
    }
  }
}
