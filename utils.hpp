#include <string>
#include <vector>

int random(int min, int max);

void print(std::string msg);
void print(int msg);
void print(float msg);
void printVector(const std::vector<int>& vec, std::string name = "Vec");
void printVector(const std::vector<std::string>& vec, std::string name = "Vec");

