#include "xos.h"
#include <iostream>
#include <string>

namespace xos
{
  std::unique_ptr<PrototypeAST> Parser::parsePrototype(xos::Result<xos::Token> tokIden)
  {
    std::string name = tokIden.get().getVal().sval;
    xos::Result<xos::Token> tok = lexer_.getNextToken();
    if (tok.get().getKind() != xos::Token::lparen)
    {
      std::cout << "Error: expected ( in prototype." << std::endl;
      return nullptr;
    }
    tok = lexer_.getNextToken();
    if (tok.get().getKind() != xos::Token::lparen)
    {
      std::cout << "Error: expected ( in prototype." << std::endl;
      return nullptr;
    }
    //[TODO] parse the input arguments here.
    tok = lexer_.getNextToken();
    if (tok.get().getKind() != xos::Token::rparen)
    {
      std::cout << "Error: expected ) in prototype." << std::endl;
      return nullptr;
    }
    tok = lexer_.getNextToken();
    if (tok.get().getKind() != xos::Token::arrow)
    {
      std::cout << "Error: expected => in prototype." << std::endl;
      return nullptr;
    }
    tok = lexer_.getNextToken();
    if (tok.get().getKind() != xos::Token::lparen)
    {
      std::cout << "Error: expected ( in prototype." << std::endl;
      return nullptr;
    }
    //[TODO] parse the return type arguments.
    tok = lexer_.getNextToken();
    if (tok.get().getKind() != xos::Token::rparen)
    {
      std::cout << "Error: expected ) in prototype." << std::endl;
      return nullptr;
    }
    tok = lexer_.getNextToken();
    if (tok.get().getKind() != xos::Token::rparen)
    {
      std::cout << "Error: expected ) in prototype." << std::endl;
      return nullptr;
    }
    return std::make_unique<PrototypeAST>(name, std::vector<std::string>(), "void"); // [Update as per input args and return args]
  };

  std::unique_ptr<FunctionAST> Parser::parseFunction(std::unique_ptr<PrototypeAST> proto)
  {
    xos::Result<xos::Token> tok = lexer_.getNextToken();
    //[TODO] check for body function here. WELP. WHAT TO DO HERE?
    if (tok.get().getKind() != xos::Token::out)
    {
      std::cout << "Error: expected out in function." << std::endl;
      return nullptr;
    }
    tok = lexer_.getNextToken();
    if (tok.get().getKind() != xos::Token::tstring)
    {
      std::cout << "Error: expected string in function." << std::endl;
      return nullptr;
    }
    std::string str = tok.get().getVal().sval;
    return std::make_unique<FunctionAST>(std::move(proto), std::make_unique<StrAST>(str));
  }

  void Parser::parse()
  {

    xos::Result<xos::Token> tok = lexer_.getNextToken();
    while (tok.get().getKind() != xos::Token::eof)
    {
      if (tok.get().getKind() == xos::Token::main)
      {
        std::unique_ptr<PrototypeAST> proto = parsePrototype(tok);
        if (proto != nullptr)
        {
          std::unique_ptr<FunctionAST> func = parseFunction(std::move(proto));
          if (func != nullptr)
          {
            std::cout << "Function is found: " << std::endl;
            std::cout << "Function name is: " << func->Proto->getName() << std::endl;
            std::cout << "Function body is: " << func->Body->StrVal << std::endl;
            return;
          }
        }
      }
    }

    return;
  }
}
