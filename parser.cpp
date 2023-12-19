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

Result<std::unique_ptr<ast::Str>> Parser::parseStr() {
  Result<Token> res = lexer_.getNextToken();
  if (res.hasError()) {
    return res;
  }
  const Token &tok = res.get();
  if (tok.getKind() != Token::string) {
    std::stringstream ss;
    ss << "Expected string on row " << tok.getRow() << ", col " << tok.getCol();
    return Result<std::unique_ptr<ast::Str>>::Error(ss.str());
  }
  return std::make_unique<ast::Str>(tok.getVal());
}

std::unique_ptr<ast::Out> Parser::parseOut() {
  Result<Token> tok = lexer_.getNextToken();
  if (tok.hasError()) return nullptr;

  if (tok.get().getKind() != Token::out) return nullptr;

  Result<std::unique_ptr<ast::Str>> res = parseStr();
  if (res.hasError()) return nullptr;

  std::unique_ptr<ast::Str> &str = res.get();
  if (!str) return nullptr;

  return std::make_unique<ast::Out>(std::move(str));
}

}  // namespace xos
