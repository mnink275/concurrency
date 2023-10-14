#include <iostream>

#include "core/thread-pool.hpp"

int main() {
  KThreadPool<int> tp(5);
  std::cout << tp.GetCapacity() << '\n';
}