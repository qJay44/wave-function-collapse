#include "utils.hpp"
#include <iostream>

int random(int min, int max) {
  return min + rand() % ((max + 1) - min);
}

template<typename T>
void print(T msg) {
  std::cout << msg << "\n";
}

void printVector(std::vector<int>& vec, std::string name = "Vector") {
  std::cout << name << " = { ";
  for (const int& item : vec) {
    std::cout << std::to_string(item) << " ";
  }
  std::cout << "}\n";
}

