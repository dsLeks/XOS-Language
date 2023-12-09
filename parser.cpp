#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <istream>
#include <memory>
#include <string>
#include <vector>

#include "xos.h"

namespace xos {

std::unique_ptr<ast::Str> Parser::parseStr() {
  Result<Token> tok = lexer_.getNextToken();
  if (tok.hasError()) {
    return nullptr;
  }
  if (tok.get().getKind() != Token::string) {
    return nullptr;
  }
  return std::make_unique<ast::Str>(tok.get().getVal());
};

std::unique_ptr<ast::Out> Parser::parseOut() {
  Result<Token> tok = lexer_.getNextToken();
  if (tok.hasError()) {
    return nullptr;
  }
  if (tok.get().getKind() != Token::out) {
    return nullptr;
  }
  std::unique_ptr<ast::Str> str = parseStr();
  if (!str) {
    return nullptr;
  }
  return std::make_unique<ast::Out>(std::move(str));
}

}  // namespace xos