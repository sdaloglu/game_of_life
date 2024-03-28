/**
 * @file grid_utils.cpp
 * @author Sabahattin Mert Daloglu (sm89@cam.ac.uk)
 * @brief 
 * @version 0.1
 * @date 2024-03-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "grid_utils.h"
#include "grid.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * @brief Function to read a binary grid from a .txt file.
 * 
 * @param filename input file name
 * @return Grid in the form of grid class
 */
Grid readGridFromFile(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {  // Check if the file is opened successfully
    throw std::runtime_error("Could not open file " + filename);
  }

  std::vector<std::string> lines;   // Vector to store the lines of the grid
  std::string line;               // String to store each line of the grid
  while (std::getline(file, line)) {   // Read the file line by line
    std::string processedLine;    
    for (char c : line) {
      if (c != '0' && c != '1') {   // Check if the character is valid
        std::ostringstream message;
        message << "Invalid character '" << c << "' detected in grid. Only '0' or '1' are allowed.";
        throw std::runtime_error(message.str());
      }
      processedLine += c;  // If the character is valid, add it to the processed line
    }
    if (!processedLine.empty()) {  
      lines.push_back(processedLine);  // If the line is not empty, add it to the vector
    }
  }

  if (lines.empty()) {  // Check if input file is empty
    throw std::runtime_error("The input file is empty or doesn't contain any valid grid representation.");
  }

  size_t expectedSize = lines[0].size();
  for (const auto &l : lines) {  // Check if all lines are of the same length (square grid)
    if (l.size() != expectedSize) {
      throw std::runtime_error("Input grid is not a square. All rows must be of the same length.");
    }
  }

  // Now that we've verified the grid is square, allocate an array for it
  int *gridArray = new int[expectedSize * expectedSize];
  for (size_t i = 0; i < expectedSize; ++i) {
    for (size_t j = 0; j < expectedSize; ++j) {
      gridArray[i * expectedSize + j] = lines[i][j] - '0';  // Allocate the grid array
    }
  }

  // Create a Grid object and use setGrid to populate it
  Grid grid(expectedSize, expectedSize, 42);
  grid.setGrid(gridArray);

  // Clean up the temporary array and return the Grid object
  delete[] gridArray;

  return grid;
}
