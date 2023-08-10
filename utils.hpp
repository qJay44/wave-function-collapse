#include <string>
#include <vector>
#include <iostream>

int random(int min, int max);

template<typename T>
T random(const std::vector<T>& vec) {
  return vec[random(0, vec.size() - 1)];
}

template<typename T>
void print(T msg) {
  std::cout << msg << "\n";
}

template<typename T>
void printVector(const std::vector<T>& vec, std::string name) {
  std::cout << name << " = { ";
  for (const T& item : vec) {
    std::cout << item << " ";
  }
  std::cout << "}\n";
}

