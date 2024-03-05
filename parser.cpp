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

Result<ast::Func> Parser::parseFunc() {
  Result<Token> res = lexer_.Lex();
  if (res.hasError()) return res;

  const Token &tok = res.get();
  if (tok.getKind() != Token::identifier) {
    return Result<ast::Func>::BuildError()
           << "Expected identifier on row " << tok.getRow() << ", col "
           << tok.getCol();
  }
  std::string name = tok.getVal();
  Result<ast::Prototype> pres = parsePrototype(name);
  if (pres.hasError()) return pres;
  Result<ast::Out> body = parseBody();
  if (body.hasError()) return body;
  return Result<ast::Func>(std::unique_ptr<ast::Prototype>(pres.Release()),
                           std::unique_ptr<ast::Out>(body.Release()));
}

Result<ast::Out> Parser::parseBody() {
  Result<Token> res = lexer_.Lex();
  if (res.hasError()) return res;
  const Token &tok = res.get();
  if (tok.getKind() == Token::out) {
    Result<ast::Str> sres = parseStr();
    if (sres.hasError()) return sres;
    return Result<ast::Out>(std::unique_ptr<ast::Str>(sres.Release()));
  }
  return Result<ast::Out>::BuildError()
         << "Expected 'out' on row " << tok.getRow() << ", col "
         << tok.getCol();
}

Result<ast::Prototype> Parser::parsePrototype(std::string name) {
  Result<Token> res = lexer_.Lex();
  if (res.hasError()) return res;
  Token *tok = &res.get();
  if (tok->getKind() != Token::lparen) {
    return Result<ast::Prototype>::BuildError()
           << "Expected '(' on row " << tok->getRow() << ", col "
           << tok->getCol();
  }

  while (tok->getKind() != Token::rparen) {
    res = lexer_.Lex();
    tok = &res.get();
  }

  res = lexer_.Lex();
  tok = &res.get();
  if (tok->getKind() != Token::arrow) {
    return Result<ast::Prototype>::BuildError()
           << "Expected '=>' on row " << tok->getRow() << ", col "
           << tok->getCol();
  }
  res = lexer_.Lex();
  tok = &res.get();

  while (tok->getKind() != Token::rparen) {
    res = lexer_.Lex();
    tok = &res.get();
  }
  res = lexer_.Lex();
  tok = &res.get();
  return Result<ast::Prototype>(name, std::vector<std::string>(), "");
}

}  // namespace xos
