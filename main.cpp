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
  while (1) {
    xos::Result<xos::Token> tok = lexer.getNextToken();
    if (tok.hasError()) {
      std::cerr << tok.getErr() << std::endl;
      return 1;
    }
    std::cout << tok.get().getKind() << " , ";

    if (tok.get().getKind() == xos::Token::eof) break;
  }

  std::cout << std::endl;
  return 0;
}
