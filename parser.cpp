#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <istream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "xos.h"

namespace xos
{

  Result<std::unique_ptr<ast::Str>> Parser::parseStr()
  {
    Result<Token> res = lexer_.Lex();
    if (res.hasError())
    {
      return res;
    }
    const Token &tok = res.get();
    if (tok.getKind() != Token::string)
    {
      return Result<std::unique_ptr<ast::Str>>::BuildError()
             << "Expected string on row " << tok.getRow() << ", col "
             << tok.getCol();
    }
    return Result<std::unique_ptr<ast::Str>>(
        std::make_unique<ast::Str>(tok.getVal()));
  }

  Result<std::unique_ptr<ast::Out>> Parser::parseOut()
  {
    Result<Token> res = lexer_.Lex();
    if (res.hasError())
      return res;

    const Token &tok = res.get();
    if (tok.getKind() != Token::out)
    {
      return Result<std::unique_ptr<ast::Out>>::BuildError()
             << "Expected 'out' on row " << tok.getRow() << ", col "
             << tok.getCol();
    }

    Result<std::unique_ptr<ast::Str>> sres = parseStr();
    if (sres.hasError())
      return sres;
    std::unique_ptr<ast::Str> &str = sres.get();
    return Result<std::unique_ptr<ast::Out>>(
        std::make_unique<ast::Out>(std::move(str)));
  }

} // namespace xos
