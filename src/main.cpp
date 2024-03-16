/**
 * @file main.cpp
 * @author Sabahattin Mert Daloglu (sm89@cam.ac.uk)
 * @brief
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

  int grid_size = std::stoi(argv[1]);  // Size of the grid, must be a square
  int time_steps = std::stoi(argv[2]); // Number of time steps to evolve the grid

  // Generate the initial state of the game -- random binary matrix (square
  // grid) of size Size 10x10 This could also be a user input file containing a
  // square grid

  // Grid grid(grid_size, grid_size, 42);

  std::string path = "./grids/";
  std::string filename = argv[3];
  std::string filepath = path + filename;
  Grid grid = readGridFromFile(filepath);
  int grid_size = grid.getSize();   // Input must be a square grid, size is both for rows and columns

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
  int n_process_x = dims[0];
  int n_process_y = dims[1];
  int process_cols, process_rows;

  if (rank == 0) {

    // Checking if the grid requires padding for the scatter
    if (grid_size % n_process_x != 0 || grid_size % n_process_y != 0) {
      int new_size_x = grid_size + (n_process_x - grid_size % n_process_x);
      int new_size_y = grid_size + (n_process_y - grid_size % n_process_y);
      grid.padding(new_size_x, new_size_y);

      // Calculate the number of columns and rows per process's grid
      process_cols = new_size_x / n_process_x;
      process_rows = new_size_y / n_process_y;
      grid.reorganizeGrid(n_process_x, n_process_y); // Reorganize the grid for contiguous memory access when scattering

      std::cout << "HERE" << std::endl;
    } else {
      int new_size_x = grid_size;
      int new_size_y = grid_size;

      // Calculate the number of columns and rows per process's grid
      process_cols = new_size_x / n_process_x;
      process_rows = new_size_y / n_process_y;
      grid.reorganizeGrid(n_process_x, n_process_y); // Reorganize the grid for contiguous memory access when scattering
    }
  }

  // Broadcast process_cols and process_rows to all processes
  MPI_Bcast(&process_cols, 1, MPI_INT, 0, cart_comm);
  MPI_Bcast(&process_rows, 1, MPI_INT, 0, cart_comm);

  Grid local_grid(process_rows, process_cols, 42); // Define a local grid for each process
  int chunk_size = process_cols * process_rows;    // The size of each chunk to be scattered

  // Scatter the grid to all processes
  MPI_Scatter(grid.getGrid(), chunk_size, MPI_INT, local_grid.getGrid(),
              chunk_size, MPI_INT, 0, cart_comm);

  // Create a convolution grid for each process
  Grid conv_grid(process_rows, process_cols, 42);
  for (int time = 0; time < time_steps; ++time) { // Run the game for 50 time steps in parallel

    // Deep copy the current local grid to the convolution grid
    conv_grid.setGrid(local_grid.getGrid());

    conv_grid.AddVerticalPadding(process_cols, process_rows); // Add vertical padding to the local grid
    conv_grid.VerticalHaloExchange(rank, nranks, cart_comm);  // Communicate the vertical halo cells with the neighboring processes

    MPI_Barrier(cart_comm);
    conv_grid.VerticalConv(process_cols, process_rows); // Update the local grid using the vertical convolution

    MPI_Barrier(cart_comm); // Wait for all processes to finish communicating the vertical halo cells

    conv_grid.AddHorizontalPadding(process_cols, process_rows); // Add horizontal padding to the local grid
    conv_grid.HorizontalHaloExchange(rank, nranks, cart_comm);  // Communicate the horizontal halo cells with the neighboring processes

    MPI_Barrier(cart_comm);
    conv_grid.HorizontalConv(process_cols, process_rows); // Update the local grid using the horizontal convolution

    MPI_Barrier(cart_comm); // Wait for all processes to finish communicating the horizontal halo cells

    local_grid.ApplyGameRules(conv_grid); // Apply the game rules to the local grid

    MPI_Barrier(cart_comm); // Wait for all processes to finish communicating the horizontal halo cells
  }

  // Gather the local grids to the global grid
  MPI_Gather(local_grid.getGrid(), chunk_size, MPI_INT, grid.getGrid(),
             chunk_size, MPI_INT, 0, cart_comm);

  if (rank == 0) {

    // Extract the original order of the grid
    grid.inverseReorganizeGrid(n_process_x, n_process_y);

    if (grid_size % n_process_x != 0 || grid_size % n_process_y != 0) {
      // Undo the padding to get the original grid
      grid.unpadGrid(grid_size);
    }

    std::cout << "_______Rank___" << rank << std::endl;
    grid.printGrid();
  }

  MPI_Barrier(cart_comm); // Wait for all processes to finish gathering the local grids
  MPI_Finalize();

  // if (std::string(argv[3]) == "cache_blocking") {
  //   // Cache blocking test

  //   std::filesystem::create_directories("./graphs");
  //   for (int blockSize = 1; blockSize <= 64; blockSize++) {
  //     timing::start_clock(); // Start the clock
  //     for (int time = 0; time < time_steps; ++time) {
  //       grid.updateGridCache(blockSize);
  //     }
  //     double elapsed_time = timing::get_split(); // Stop the clock

  //     // Path to save the time taken for different block sizes
  //     std::string filepath = "./graphs/cache_blocking_time.txt";

  //     std::ofstream file(filepath, std::ios::app); // Append to the file
  //     if (file.is_open()) {
  //       file << "Block size: " << blockSize << " Time: " << elapsed_time
  //            << " ms" << std::endl;
  //       file.close();
  //     } else {
  //       std::cerr << "Failed to open file at " << filepath << std::endl;
  //     }
  //   }
  // }

  // else if (std::string(argv[3]) == "time") {
  //   // Run the game for input time steps only for visualization
  //   timing::start_clock(); // Start the clock
  //   for (int time = 0; time < time_steps; ++time) {
  //     std::cout << "Time Step " << time << std::endl;
  //     std::cout << "____________________" << std::endl;
  //     grid.updateGridConvolve();
  //   }
  //   double elapsed_time = timing::get_split(); // Stop the clock
  //   std::cout << "Time taken for updating grid of size " << grid_size << "x"
  //             << grid_size << " over " << time_steps
  //             << " time steps is: " << elapsed_time << " ms"
  //             << std::endl; // Print the elapsed time
  // }

  return 0;
}
