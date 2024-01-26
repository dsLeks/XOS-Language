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

namespace xos {

Result<ast::Str> Parser::parseStr() {
  Result<Token> res = lexer_.Lex();
  if (res.hasError()) {
    return res;
  }
  const Token &tok = res.get();
  if (tok.getKind() != Token::string) {
    return Result<ast::Str>::BuildError()
           << "Expected string on row " << tok.getRow() << ", col "
           << tok.getCol();
  }
  return Result<ast::Str>(tok.getVal());
}

Result<ast::Out> Parser::parseOut() {
  Result<Token> res = lexer_.Lex();
  if (res.hasError()) return res;

  const Token &tok = res.get();
  if (tok.getKind() != Token::out) {
    return Result<ast::Out>::BuildError()
           << "Expected 'out' on row " << tok.getRow() << ", col "
           << tok.getCol();
  }

  Result<ast::Str> sres = parseStr();
  if (sres.hasError()) return sres;
  std::unique_ptr<ast::Str> str(sres.Release());
  return Result<ast::Out>(std::move(str));
}

}  // namespace xos
