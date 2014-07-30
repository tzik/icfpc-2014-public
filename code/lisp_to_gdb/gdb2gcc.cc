#include "emulator/gcc_vm.h"

#include <iostream>

int main(int, char**) {
  gcc::Program program = gcc::LoadProgram(std::cin);
  gcc::SaveProgram(program, std::cout);
  return 0;
}
