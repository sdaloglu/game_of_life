// Timing functions here

// Adapted from Dr. James Fergusson's code

#include "timing.h"

#include <chrono>

std::chrono::high_resolution_clock::time_point time_point;

void timing::start_clock() {
  time_point = std::chrono::high_resolution_clock::now();
}

double timing::get_split() {
  std::chrono::high_resolution_clock::time_point time_split =
      std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> duration = time_split - time_point;
  time_point = time_split;
  return duration.count();
}