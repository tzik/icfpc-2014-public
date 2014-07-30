#include "converter.h"

#include <cassert>
#include <cmath>
#include <deque>
#include <iostream>
#include <string>

#include "parser.h"
#include "tokenizer.h"
#include "preprocessor.h"

int main() {
  try {
    std::string program = PreProcessor::PreProcess(std::cin);
    std::deque<std::string> tokens = Tokenizer::Tokenize(program);
    Parser parser;
    Value* prog = parser.Parse(tokens);
    std::cout << Compile(prog) << std::endl;

    // Warning defined / unused functions.
    const auto& unused = parser.GetUnusedFunctions();
    if (!unused.empty()) {
      std::cerr << "Warning: Following functions are not called:\n";
      for (const auto& func : unused) {
        std::cerr << "   " << func << "\n";
      }
    }

    // "#INCLUDE" undefined functions automatically.
    const auto& undef = parser.GetUndefinedFunctions();
    for (const auto& func : undef)
      std::cout << "#INCLUDE lib/" << func << ".gdb\n";
  } catch (const std::string& message) {
    std::cerr << "Compile error: " << message << std::endl;
  } catch (const char* message) {
    std::cerr << "Compile error: " << message << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  } catch (...) {
    std::cerr << "Unknown exception" << std::endl;
  }
  return 0;
}

