#include "utils.hpp"
#include <iostream>
#include <string>

int random(int min, int max) {
  return min + rand() % ((max + 1) - min);
}

void print(std::string msg) {
  std::cout << msg << "\n";
}

void print(int msg) {
  std::cout << std::to_string(msg) << "\n";
}

void printVector(std::vector<int>& vec, std::string name = "Vector") {
  std::cout << name << " = { ";
  for (const int& item : vec) {
    std::cout << std::to_string(item) << " ";
  }
  std::cout << "}\n";
}

