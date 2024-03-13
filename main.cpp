#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <string>

#include "xos.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Missing input file" << std::endl;
    return 1;
  }

  std::ifstream input(argv[1]);
  xos::Lexer lexer(input);
  xos::Parser parser(lexer);
  auto func = parser.parseFunc();
  xos::Compiler compiler;
  compiler.compile(*func);
  compiler.TheModule->print(llvm::outs(), nullptr);
  std::cout << std::endl;
  return 0;
}
