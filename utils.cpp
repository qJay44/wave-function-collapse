#include "utils.hpp"
#include <iostream>

int random(int min, int max) {
  const int diff = max - min + 1;

  if (diff == 0)
    return min;

  return rand() % (diff) + min;
}

void print(std::string msg) {
  std::cout << msg << "\n";
}

void print(int msg) {
  std::cout << std::to_string(msg) << "\n";
}

void printVector(const std::vector<int>& vec, std::string name = "Vector") {
  std::cout << name << " = { ";
  for (const int& item : vec) {
    std::cout << std::to_string(item) << " ";
  }
  std::cout << "}\n";
}

