#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <string>

#include "xos.h"

namespace xos {

Lexer::Lexer(std::istream &input) : input_(input) {
  assert(input_.good() && "Unable to open input stream.");
}

int Lexer::getNextChar() {
  int ch;
  if (has_lookahead_) {
    has_lookahead_ = false;
    ch = lookahead_;
  } else {
    ch = input_.get();
  }

  if (ch == EOF) return ch;

  if (ch == '\n') {
    row_++;
    col_ = 1;
  } else {
    col_++;
  }
  return ch;
}

Result<Token> Lexer::getNextToken() {
  char ch = getNextChar();

  // skip the whitespace
  while (isspace(ch)) {
    ch = getNextChar();
  }

  if (ch == EOF) return makeResult(Token::eof);

  if (ch == '=') {
    ch = getNextChar();
    if (ch == '>') {
      return makeResult(Token::arrow);
    } else {
      std::ostringstream ss;
      ss << "Expected '=>' at row " << row_ << ", col " << col_;
      return Result<Token>::Error(ss.str());
    }
  }
  if (ch == '(') {
    return makeResult(Token::lparen);
  }
  if (ch == ')') {
    return makeResult(Token::rparen);
  }

  if (ch == '"') {
    IdentifierStr = "";
    ch = getNextChar();
    while (ch != '"') {
      IdentifierStr += ch;
      ch = getNextChar();
    }
    return makeResult(Token::string);
  }

  if (((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) && ch != EOF) {
    IdentifierStr = "";
    while (((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) &&
           ch != EOF) {
      IdentifierStr += ch;
      ch = getNextChar();
    }

    // Re-add the last popped character onto the buffer.
    lookahead_ = ch;
    has_lookahead_ = true;

    if (IdentifierStr == "main") {
      return makeResult(Token::main);
    }

    if (IdentifierStr == "out") {
      return makeResult(Token::out);
    }
  }

  return makeResult(Token::identifier);
}

}  // namespace xos
