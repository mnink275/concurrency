#include <iostream>

#include "core/thread-pool.hpp"

int main() {
  ink::KThreadPool<int> tp(5);
  std::cout << tp.GetCapacity() << '\n';
}
