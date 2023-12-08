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

    if (ch == '\n') {
      row_++;
      col_ = 0;
    } else {
      col_++;
    }
  }
  
  return ch;
}

Result<Token> Lexer::getNextToken() {
  char ch = getNextChar();
  uint32_t row = row_, col = col_;

  // skip the whitespace
  while (isspace(ch)) {
    ch = getNextChar();
    row = row_;
    col = col_;
  }

  if (ch == EOF) return Token(Token::eof, row, col);

  if (ch == '=') {
    ch = getNextChar();
    if (ch == '>') {
      return Token(Token::arrow, row, col);
    } else {
      std::ostringstream ss;
      ss << "Expected '=>' at row " << row_ << ", col " << col_;
      return Result<Token>::Error(ss.str());
    }
  }
  if (ch == '(') {
    return Token(Token::lparen, row, col);
  }
  if (ch == ')') {
    return Token(Token::rparen, row, col);
  }

  if (ch == '"') {
    IdentifierStr = "";
    ch = getNextChar();
    while (ch != '"') {
      IdentifierStr += ch;
      ch = getNextChar();
    }
    return Token(Token::string, row, col);
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
      return Token(Token::main, row, col);
    }

    if (IdentifierStr == "out") {
      return Token(Token::out, row, col);
    }
  }

  return Token(Token::identifier, row, col);
}

}  // namespace xos
