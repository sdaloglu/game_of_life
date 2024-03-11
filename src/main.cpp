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

#include <chrono>
#include <iostream>  //for input and output
#include <thread>

#include "grid.h"
#include "timing.h"

/**
 * @brief The main function for the Conway's Game of Life with periodic boundary
 * conditions.
 *
 * @param argc int, the number of command line arguments
 * @param argv char*, the command line arguments
 * @return int the exit code of the program
 */
int main(int argc, char* argv[]) {
  // MPI_Init(&argc, &argv);
  // int rank, nranks;
  // MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  // MPI_Comm_size(MPI_COMM_WORLD, &nranks);

  // Input the size of the grid from the command line
  int grid_size = std::stoi(argv[1]);  // Size of the grid

  //
  // int process_sqrt = static_cast<int>(sqrt(nranks));  // Cast to int
  // Check if nrank is a perfect square
  // if (process_sqrt * process_sqrt != nranks) {
  //   std::cerr << "The number of processes must be a perfect square"
  //             << std::endl;
  //   return 1;
  // }

  // Check if the grid size is divisible by the number of processes
  // if (grid_size % process_sqrt != 0) {
  //   std::cerr << "The size of the grid must be divisible by the square root
  //   of "
  //                "the number of processes"
  //             << std::endl;
  //   return 1;
  // }

  // Generate the initial state of the game -- random binary matrix (square
  // grid) of size Size 10x10 This could also be a user input file containing a
  // square grid
  Grid grid(grid_size, grid_size, 42);
  // Print the initial grid
  // for (int i = 0; i < grid_size; i++) {
  //   for (int j = 0; j < grid_size; j++) {
  //     std::cout << grid(i, j) << " ";
  //   }
  //   std::cout << std::endl;
  // }

  // Reoranize the grid so that each chunk to be scattered is contiguous in
  // memory
  // grid.reorganizeGrid(nranks);
  // Put a test here to see if the grid is reorganized correctly

  // The size of each chunk to be scattered
  // int chunk_size = grid_size / sqrt(nranks);

  // Define a local buffer to receive the scattered grid -- randomly filled
  // Grid local_grid(chunk_size, chunk_size, 5);

  // Scatter the grid to all processes
  // MPI_Scatter(grid.getGrid(), chunk_size * chunk_size, MPI_INT,
  //             local_grid.getGrid(), chunk_size * chunk_size, MPI_INT, 0,
  //             MPI_COMM_WORLD);

  // Run the game for 50 time steps in parallel
  // for (int time = 0; time < 50; ++time) {
  //   // Communicate the boundary cells with the neighboring processes
  //   // Halo exchange of size 1 is required -- also periodic boundary
  //   conditions

  //   local_grid.communicateBoundary(rank, nranks);

  //   // Do a local update of the grid here
  //   local_grid.updateGrid();
  //   // MPI_Barrier(MPI_COMM_WORLD);

  //   // To plot the local grid
  //   if (rank == 20) {
  //     std::cout << "Time Step " << time << std::endl;
  //     std::cout << "____________________" << std::endl;
  //     local_grid.printGrid();
  //     std::cout << "____________________" << std::endl;
  //     std::this_thread::sleep_for(
  //         std::chrono::milliseconds(500));  // Delay for 500ms
  //     system("clear");                      // Clear the console
  //   }
  // }

  // Gather the local grids to the global grid
  // MPI_Gather(local_grid.getGrid(), chunk_size * chunk_size, MPI_INT,
  //            grid.getGrid(), chunk_size * chunk_size, MPI_INT, 0,
  //            MPI_COMM_WORLD);

  // MPI_Finalize();

  // Run the game for 50 time steps only for visualization
  for (int time = 0; time < 50; ++time) {
    std::cout << "Time Step " << time << std::endl;
    std::cout << "____________________" << std::endl;
    // grid.printGrid();
    timing::start_clock();  // Start the clock
    grid.updateGrid();
    double elapsed_time = timing::get_split();  // Stop the clock
    std::cout << "Time taken for updating the grid: " << elapsed_time << " ms"
              << std::endl;
    std::cout << "____________________" << std::endl;
    // std::this_thread::sleep_for(
    //     std::chrono::milliseconds(500));  // Delay for 500ms
    // system("clear");                      // Clear the console
  }

  return 0;
}
