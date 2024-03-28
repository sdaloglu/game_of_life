/**
 * @file main.cpp
 * @author Sabahattin Mert Daloglu (sm89@cam.ac.uk)
 * @brief The main function for the Conway's Game of Life with periodic boundary
 * @version 0.1
 * @date 2024-03-11
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "grid.h"
#include "grid_utils.h"
#include "timing.h"
#include <chrono>
#include <fstream>
#include <iostream> //for input and output
#include <thread>

/**
 * @brief The main function for the Conway's Game of Life with periodic boundary
 * conditions.
 *
 * @param argc int, the number of command line arguments
 * @param argv char*, the command line arguments
 * @return int the exit code of the program
 */
int main(int argc, char *argv[]) {

  MPI_Init(&argc, &argv);

  int rank, nranks;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nranks);

  // Generate the initial state of the game -- random binary matrix (square
  // grid). This could also be a user input file containing a
  // square grid

  int grid_size; // Declare the size of the outside the if-else block
  Grid grid;     // Declare the grid
  // Check if the second argument ends with .txt
  if (std::string(argv[1]).substr(std::string(argv[1]).find_last_of(".") + 1) == "txt") {
    // Input is a txt file containing a square grid (hopefully)
    std::cout << "Reading grid from file" << std::endl;
    std::string filename = argv[1];
    std::string path = "./grids/";
    std::string filepath = path + filename;
    grid = readGridFromFile(filepath);
    grid_size = grid.getSize(); // Input must be a square grid, size is both for rows and columns

  } else {
    // Input is the size of the grid, we will generate a random grid
    std::cout << "Generating random grid with size " << argv[1] << "x" << argv[1] << std::endl;
    grid_size = std::stoi(argv[1]); // Size of the grid, must be a square
    grid = Grid(grid_size, grid_size, 42);
  }

  int time_steps = std::stoi(argv[2]); // Number of time steps to evolve the grid

  MPI_Comm cart_comm; // Define a new communicator for the 2D grid of processes
  // Using the Cartesian Communicator to create a 2D grid of processes
  int dims[2] = {0, 0};    // Specify the number of processes in each dimension, 0
                           // means that MPI_Cart_create will determine the number
                           // of processes in each dimension
  int periods[2] = {1, 1}; // Periodic boundaries in both dimensions (wrap around)
  int reorder = 1;         // Allow processes to be reordered for efficiency
  int coords[2];           // Coordinates of the current process

  MPI_Dims_create(nranks, 2, dims);                                       // Determine the number of processes in each dimension
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &cart_comm); // Create a 2D Cartesian communicator
  MPI_Cart_get(cart_comm, 2, dims, periods, coords);                      // Get the dimensions and coordinates of the Cartesian communicator
  // print dimensions
  if (rank == 0) {
    std::cout << "The grid is divided into " << dims[0] << "x" << dims[1] << " processes" << std::endl;
  }
  int n_process_x = dims[0];   // Number of processes in the x dimension
  int n_process_y = dims[1];   // Number of processes in the y dimension
  int process_cols, process_rows;
  int remaining_rows, remaining_cols, full_rows, full_cols = 0;   // Initialize variables to store the remaining rows and columns
  int *sendcounts = new int[nranks];  // This is an array to store the uneven number of elements to be sent to each process (used in MPI_Scatterv)
  int *displs = new int[nranks];    // This is an array to store the uneven jumps in the send buffer (used in MPI_Scatterv)

  if (rank == 0) {
    // Instead of padding the grid, assign the remaining rows and columns to the last processes

    // (n_process_x - 1) * n_process_y is the number of processes that will have full rows: full_rows
    full_rows = grid_size / n_process_x;  // Number of full rows for each process (baseline)
    // n_process_y number of processes will have the remaining rows: remaining_rows
    remaining_rows = grid_size % n_process_x;   // Number of remaining rows for the last processes (if any)

    // (n_process_y - 1) * n_process_x is the number of processes that will have full columns: full_cols
    full_cols = grid_size / n_process_y;  // Number of full columns for each process (baseline)
    // n_process_x number of processes will have the remaining columns: remaining_cols
    remaining_cols = grid_size % n_process_y;  // Number of remaining columns for the last processes (if any)

    // Fill the sendcounts array so that the last columns and rows of the the process grid will have the remaining rows and columns
    // The rest of the processes will have full rows and columns
    // The number of processes with the remaining rows and columns is n_process_x + n_process_y - 1
    for (int i = 0; i < n_process_x; ++i) {
      for (int j = 0; j < n_process_y; ++j) {
        // Determine the number of rows for the current process
        int rows_for_process = (i < n_process_x - 1) ? full_rows : full_rows + remaining_rows;
        // Determine the number of columns for the current process
        int cols_for_process = (j < n_process_y - 1) ? full_cols : full_cols + remaining_cols;

        // Calculating the sendcount for the current process
        sendcounts[i * n_process_y + j] = rows_for_process * cols_for_process;
      }
    }

    // Creating displs array for MPI_Scatterv from sendcounts (cumulative sum of sendcounts)
    int disp = 0;
    for (int i = 0; i < nranks; i++) {
      displs[i] = disp;
      disp += sendcounts[i];
    }
  }

  grid.reorganizeGrid(n_process_x, n_process_y, grid_size); // Reorganize the grid in a contiguous memory layout, prepare for scatterv

  // Broadast remaining rows and columns to all processes
  MPI_Bcast(&remaining_rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&remaining_cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
  // Broadcast full rows and columns to all processes
  MPI_Bcast(&full_rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&full_cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
  // Boradcast the sendcounts and displs to all processes
  MPI_Bcast(sendcounts, nranks, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(displs, nranks, MPI_INT, 0, MPI_COMM_WORLD);

  // Determine local grid size for each process (whether it has remaining rows and columns or not)
  int local_rows = (coords[0] == dims[0] - 1 && remaining_rows > 0) ? full_rows + remaining_rows : full_rows;
  int local_cols = (coords[1] == dims[1] - 1 && remaining_cols > 0) ? full_cols + remaining_cols : full_cols;

  // Create a local grid for each process
  Grid local_grid(local_rows, local_cols, 42);
  // Determine the recvcount for the local grid
  int recvcount = local_rows * local_cols;

  // Scatter the grid to all processes
  MPI_Scatterv(rank == 0 ? grid.getGrid() : nullptr, sendcounts, displs, MPI_INT, local_grid.getGrid(), recvcount, MPI_INT, 0, cart_comm);

  // Create a convolution grid for each process to store the convolution results
  Grid conv_grid(local_rows, local_cols, 42);

  for (int time = 0; time < time_steps; ++time) { // Run the game for some time steps in parallel

    // Deep copy the current local grid to the convolution grid
    conv_grid.setGrid(local_grid.getGrid());


    conv_grid.AddVerticalPadding(); // Add vertical padding to the local grid
    conv_grid.VerticalHaloExchange(rank, nranks, cart_comm); // Communicate the vertical halo cells with the neighboring processes
    MPI_Barrier(cart_comm);
    conv_grid.VerticalConv(); // Update the local grid using the vertical convolution

    MPI_Barrier(cart_comm); // Wait for all processes to finish communicating the vertical halo cells

    conv_grid.AddHorizontalPadding();                          // Add horizontal padding to the local grid
    conv_grid.HorizontalHaloExchange(rank, nranks, cart_comm); // Communicate the horizontal halo cells with the neighboring processes
    MPI_Barrier(cart_comm);
    conv_grid.HorizontalConv(); // Update the local grid using the horizontal convolution

    MPI_Barrier(cart_comm); // Wait for all processes to finish communicating the horizontal halo cells

    local_grid.ApplyGameRules(conv_grid); // Apply the game rules to the local grid

    MPI_Barrier(cart_comm); // Wait for all processes to finish communicating the horizontal halo cells
  }

  // Gather the local grids to the global grid
  MPI_Gatherv(local_grid.getGrid(), recvcount, MPI_INT, grid.getGrid(), sendcounts, displs, MPI_INT, 0, cart_comm);

  if (rank == 0) {

    // Extract the original order of the grid -- undo the contiguous memory layout organization
    grid.inverseReorganizeGrid(n_process_x, n_process_y, grid_size);
    
    if (std::string(argv[3]) ==  "visualize"){
      // Print the final updated grid to the console
      std::cout << "Final grid after " << time_steps << " time steps:" << std::endl;
      grid.printGrid();
    }
    else if (std::string(argv[3]) == "save"){
      // Save the final updated grid to a file
      std::string filename = "final_grid.txt";
      std::string path = "./grids/";
      std::string filepath = path + filename;
      std::ofstream file(filepath);
      if (file.is_open()) {
        for (int i = 0; i < grid_size; ++i) {
          for (int j = 0; j < grid_size; ++j) {
            file << grid(i, j);
          }
          file << std::endl;
        }
        file.close();
      } else {
        std::cerr << "Failed to open file at " << filepath << std::endl;
    }
    }

  }

  MPI_Barrier(cart_comm); // Wait for all processes to finish gathering the local grids
  MPI_Finalize();

  // Delete the sendcounts and displs arrays
  delete[] sendcounts;
  delete[] displs;
  return 0;
}