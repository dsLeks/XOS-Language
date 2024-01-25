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

  Result<ast::Str> Parser::parseStr()
  {
    Result<Token> res = lexer_.Lex();
    if (res.hasError())
    {
      return res;
    }
    const Token &tok = res.get();
    if (tok.getKind() != Token::string)
    {
      return Result<ast::Str>::BuildError()
             << "Expected string on row " << tok.getRow() << ", col "
             << tok.getCol();
    }
    return Result<ast::Str>(tok.getVal());
  }

  Result<ast::Out> Parser::parseOut()
  {
    Result<Token> res = lexer_.Lex();
    if (res.hasError())
      return res;

    const Token &tok = res.get();
    if (tok.getKind() != Token::out)
    {
      return Result<ast::Out>::BuildError()
             << "Expected 'out' on row " << tok.getRow() << ", col "
             << tok.getCol();
    }

    Result<ast::Str> sres = parseStr();
    if (sres.hasError())
      return sres;
    std::unique_ptr<ast::Str> str(sres.Release());
    return Result<ast::Out>(std::move(str));
  }

  Result<std::unique_ptr<ast::Expr>> Parser::parseExpr()
  {
    Result<Token> res = lexer_.Lex();
    if (res.hasError())
      return res;
    Token &tok = res.get();
    if (tok.getKind() == Token::out)
    {
      Result<std::unique_ptr<ast::Out>> out = parseOut();
      if (out.hasError())
        return out;
      return Result<std::unique_ptr<ast::Expr>>(std::move(out.get()));
    }

    return Result<std::unique_ptr<ast::Expr>>::BuildError()
           << "Expected '(' on row " << tok.getRow() << ", col " << tok.getCol()
           << "!!!";
    ; // TODO(dsLeks): Implement this.
  }

  Result<std::unique_ptr<ast::Prototype>> Parser::parsePrototype(
      std::string name)
  {
    Result<Token> res = lexer_.Lex();
    if (res.hasError())
      return res;
    Token &tok = res.get();
    if (tok.getKind() != Token::lparen)
    {
      return Result<std::unique_ptr<ast::Prototype>>::BuildError()
             << "Expected '(' on row " << tok.getRow() << ", col "
             << tok.getCol();
    }
    // PARSE RETURN TYPE HERE
    while (tok.getKind() != Token::rparen)
    {
      res = lexer_.Lex();
      tok = res.get();
    }
    // PARSE ARROW
    res = lexer_.Lex();
    tok = res.get();
    if (tok.getKind() != Token::arrow)
    {
      return Result<std::unique_ptr<ast::Prototype>>::BuildError()
             << "Expected '=>' on row " << tok.getRow() << ", col "
             << tok.getCol();
    }
    res = lexer_.Lex();
    tok = res.get();
    // PARSE ARGUMENTS HERE
    while (tok.getKind() != Token::rparen)
    {
      res = lexer_.Lex();
      tok = res.get();
    }
    res = lexer_.Lex();
    tok = res.get();
    return Result<std::unique_ptr<ast::Prototype>>(
        std::make_unique<ast::Prototype>(name, std::vector<std::string>(), ""));
  }

  Result<std::unique_ptr<ast::Func>> Parser::parseFunc()
  {
    Result<Token> res = lexer_.Lex();
    if (res.hasError())
      return res;

    const Token &tok = res.get();
    Result<std::unique_ptr<ast::Prototype>> pres = nullptr;
    if (tok.getKind() == Token::identifier)
    {
      std::string name = tok.getVal();
      pres = parsePrototype(name);
      if (pres.hasError())
        return pres;
    }
    Result<std::unique_ptr<ast::Expr>> exptr = parseExpr();
    if (exptr.hasError())
      return exptr;

    return Result<std::unique_ptr<ast::Func>>(std::make_unique<ast::Func>(
        std::move(pres.get()), std::move(exptr.get())));
  }

  Result<std::unique_ptr<ast::Expr>> Parser::parseExpr()
  {
    Result<Token> res = lexer_.Lex();
    if (res.hasError())
      return res;
    Token &tok = res.get();
    if (tok.getKind() == Token::out)
    {
      Result<std::unique_ptr<ast::Out>> out = parseOut();
      if (out.hasError())
        return out;
      return Result<std::unique_ptr<ast::Expr>>(std::move(out.get()));
    }

    // TODO(dsLeks): Implement this.
    return Result<std::unique_ptr<ast::Expr>>::BuildError()
           << "Unable to parse expression for token `" << tok.getKind()
           << "` on row " << tok.getRow() << ", col " << tok.getCol();
  }

  Result<std::unique_ptr<ast::Prototype>> Parser::parsePrototype(
      std::string &name)
  {
    Result<Token> res = lexer_.Lex();
    if (res.hasError())
      return res;
    Token tok = res.get();
    if (tok.getKind() != Token::lparen)
    {
      return Result<std::unique_ptr<ast::Prototype>>::BuildError()
             << "Expected '(' on row " << tok.getRow() << ", col "
             << tok.getCol();
    }
    while (tok.getKind() != Token::rparen)
    {
      res = lexer_.Lex();
      tok = res.get();
    }

    res = lexer_.Lex();
    tok = res.get();
    if (tok.getKind() != Token::arrow)
    {
      return Result<std::unique_ptr<ast::Prototype>>::BuildError()
             << "Expected '=>' on row " << tok.getRow() << ", col "
             << tok.getCol();
    }
    res = lexer_.Lex();
    tok = res.get();

    while (tok.getKind() != Token::rparen)
    {
      res = lexer_.Lex();
      tok = res.get();
    }
    res = lexer_.Lex();
    tok = res.get();
    return Result<std::unique_ptr<ast::Prototype>>(
        std::make_unique<ast::Prototype>(name, std::vector<std::string>(), ""));
  }

  Result<std::unique_ptr<ast::Func>> Parser::parseFunc()
  {
    Result<Token> res = lexer_.Lex();
    if (res.hasError())
      return res;

    const Token &tok = res.get();
    Result<std::unique_ptr<ast::Prototype>> pres = nullptr;
    if (tok.getKind() == Token::identifier)
    {
      std::string name = tok.getVal();
      pres = parsePrototype(name);
      if (pres.hasError())
        return pres;
    }
    Result<std::unique_ptr<ast::Expr>> expr = parseExpr();
    if (expr.hasError())
      return expr;

    return Result<std::unique_ptr<ast::Func>>(std::make_unique<ast::Func>(
        std::move(pres.get()), std::move(expr.get())));
  }

} // namespace xos
