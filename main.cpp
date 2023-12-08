#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <string>

#include "xos.h"

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    std::cerr << "Missing input file" << std::endl;
    return 1;
  }

  std::ifstream input(argv[1]);
  // xos::Lexer lexer(input);
  // xos::Result<xos::Token> tok{xos::Token::eof, 0, 0, xos::Token::val{.eof = true}}; // set to a random value -- for now for testing.
  xos::Parser parser(input);
  parser.parse();
  // while (1)
  // {
  //   tok = lexer.getNextToken();
  //   if (tok.hasError())
  //   {
  //     std::cerr << tok.getErr() << std::endl;
  //     return 1;
  //   }
  //   auto tok_kind = tok.get().getKind();
  //   if (tok.get().getKind() == xos::Token::eof)
  //   {
  //     std::cout << tok.get().getVal().eof << std::endl;
  //     break;
  //   }

  //   switch (tok_kind)
  //   {
  //   case xos::Token::eof:
  //     std::cout << tok.get().getVal().eof << std::endl;
  //     break;
  //   case xos::Token::main:
  //     std::cout << tok.get().getVal().sval << std::endl;
  //     break;
  //   case xos::Token::out:
  //     std::cout << tok.get().getVal().sval << std::endl;
  //     break;
  //   case xos::Token::lparen:
  //     std::cout << tok.get().getVal().sval << std::endl;
  //     break;
  //   case xos::Token::rparen:
  //     std::cout << tok.get().getVal().sval << std::endl;
  //     break;
  //   case xos::Token::arrow:
  //     std::cout << tok.get().getVal().sval << std::endl;
  //     break;
  //   case xos::Token::identifier:
  //     std::cout << tok.get().getVal().sval << std::endl;
  //     break;
  //   case xos::Token::tstring:
  //     std::cout << tok.get().getVal().sval << std::endl;
  //     break;
  //   default:
  //     std::cout << "error in input?" << std::endl;
  //     break;
  //   }
  // }
  return 0;
}
