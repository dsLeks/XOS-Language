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

}  // namespace xos