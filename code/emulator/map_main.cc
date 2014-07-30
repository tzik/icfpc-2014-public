#include "map.h"

#include <iostream>
#include <string>
#include <vector>

int main(void) {
  std::vector<std::string> lines;
  for (std::string line;
       std::getline(std::cin, line);) {
    if (line.empty())
      continue;
    lines.push_back(line);
  }

  Map map;
  map.Parse(lines);
  std::cout << map.GetDebugDump() << "\n";
  
  return 0;
}
