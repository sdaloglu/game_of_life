#include <iostream>    //for input and output
#include "grid.h"
#include <chrono>
#include <thread>
#include <mpi.h>

int main(int argc, char* argv[]){
    /*
        The main function for the Conway's Game of Life with periodic boundary conditions.

        Args:
            argc: int, the number of command line arguments
            argv: char*, the command line arguments

        Returns:
            int, the exit code of the program
    */
    

    MPI_Init(&argc, &argv);
    int rank, nranks;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);



    // Generate the initial state of the game -- random binary matrix (square grid) of size Size 10x10
    // This could also be a user input file containing a square grid

    int grid_size = std::stoi(argv[1]);    // Size of the grid
    Grid grid(grid_size,grid_size,5);
   

    // Split the grid into smaller grids (chunks) for each process
    int chunk_size = grid_size / nranks;    // assume the size of the grid is divisible by the number of processes

    if (grid_size % nranks != 0){
        std::cerr << "The size of the grid must be divisible by the number of processes" << std::endl;
        return 1;
    }


    if (rank == 0){

        // Scatter the grid to all processes
        MPI_Scatter(grid.data(), chunk_size*grid_size, MPI_INT, 
                    local_grid.data(), chunk_size*grid_size, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
    }
    else{
        //Define a local buffer to receive the scattered grid -- randomly filled
        Grid local_grid(chunk_size, 5);

        MPI_Scatter(NULL, chunk_size*grid_size, MPI_INT, 
                    local_grid.data(), chunk_size*grid_size, MPI_INT, 0, MPI_COMM_WORLD);

        // Run the game for 50 time steps in parallel
        for (int time=0;time<50;++time){
            // Communicate the boundary cells with the neighboring processes
            // Halo exchange of size 1 is required -- also periodic boundary conditions
            // ...
            // ...
            // ...


            // Do a local update of the grid here
            local_grid.updateGrid();

        }
        // Gather the local grids to the global grid
            MPI_Gather(local_grid.data(), chunk_size*grid.size, MPI_INT, 
                        grid.data(), chunk_size*grid.size, MPI_INT, 0, MPI_COMM_WORLD);
    }
    


    // Run the game for 50 time steps only for visualization
    for (int time = 0; time < 50; ++time){
        std::cout << "Time Step " << time << std::endl;
        std::cout << "____________________" << std::endl; 
        grid.printGrid();
        grid.updateGrid();
        std::cout << "____________________" << std::endl;  
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Delay for 500ms
        system("clear"); // Clear the console
    }


    MPI_Finalize();
    return 0;
}

    