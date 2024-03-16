#include "grid_utils.h"
#include "grid.h"
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

Grid readGridFromFile(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file " + filename);
  }

  std::vector<std::string> lines;
  std::string line;
  while (std::getline(file, line)) {
    std::string processedLine;
    for (char c : line) {
      if (c == '0' || c == '1') {
        processedLine += c;
      }
    }
    if (!processedLine.empty()) {
      lines.push_back(processedLine);
    }
  }

  if (lines.empty()) {
    throw std::runtime_error("The input file is empty or doesn't contain any valid grid representation.");
  }

  // Ensure all lines are of the same length
  size_t expectedSize = lines[0].size();
  for (const auto &l : lines) {
    if (l.size() != expectedSize) {
      throw std::runtime_error("Input grid rows are not of equal length.");
    }
  }

  // Now that we've verified the grid is square, allocate an array for it
  int *gridArray = new int[expectedSize * expectedSize];
  for (size_t i = 0; i < expectedSize; ++i) {
    for (size_t j = 0; j < expectedSize; ++j) {
      gridArray[i * expectedSize + j] = lines[i][j] - '0';
    }
  }

  // Create a Grid object and use setGrid to populate it
  Grid grid(expectedSize, expectedSize, 42);
  grid.setGrid(gridArray);

  // Clean up
  delete[] gridArray;

  return grid;
}

