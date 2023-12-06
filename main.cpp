#include <stdlib.h>

#include <iostream>
#include <string>

#include "xos.h"

int main() {
  xos::Lexer lexer("test_input.txt");
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
