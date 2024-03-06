#include <iostream>    //for input and output
#include "grid.h"
#include <chrono>
#include <thread>


int main(int argc, char* argv[]){
    /*
        The main function for the Conway's Game of Life with periodic boundary conditions.

        Args:
            argc: int, the number of command line arguments
            argv: char*, the command line arguments

        Returns:
            int, the exit code of the program
    */
    

    // Generate the initial state of the game -- random binary matrix (square grid) of size Size 10x10
    // This could also be a user input
    Grid grid(50,5);

    std::cout << "Works fine!" << std::endl;   

    for (int time = 0; time < 50; ++time){
        std::cout << "Time Step " << time << std::endl;
        std::cout << "____________________" << std::endl; 
        grid.printGrid();
        grid.updateGrid();
        std::cout << "____________________" << std::endl;  
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Delay for 500ms
        system("clear"); // Clear the console
    }


    return 0;
}

    