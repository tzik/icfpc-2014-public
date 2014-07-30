
#include "gcc_vm.h"

#include <iostream>

int main(int, char**) {
  gcc::Program program = gcc::LoadProgram(std::cin);
  gcc::VM vm;

  try {
    int time_limit = 10000;
    while (vm.is_running && --time_limit)
      vm.Step(program);
    if (!time_limit)
      std::cerr << "Time Limit Exceeded.\n";
    vm.Dump(std::cout);
  } catch(const std::runtime_error& error) {
    std::cerr << "Runtime Error: " << error.what() << "\n";
  }
  return 0;
}
