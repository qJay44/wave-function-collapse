#include "utils.hpp"

int random(int min, int max) {
  const int diff = max - min + 1;

  if (diff == 0)
    return min;

  return rand() % (diff) + min;
}

