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
      ss << "Expected '=>' at row " << row << ", col " << col;
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
    std::string quoted_str;
    ch = getNextChar();
    while (ch != '"') {
      quoted_str += ch;
      ch = getNextChar();
    }
    return Token(Token::string, row, col, quoted_str);
  }

  std::string identifier;
  if (isalpha(ch)) {
    while (isalpha(ch)) {
      identifier += ch;
      ch = getNextChar();
    }

    // Re-add the last popped character onto the buffer.
    lookahead_ = ch;
    has_lookahead_ = true;

    if (identifier == "out") return Token(Token::out, row, col);

    return Token(Token::identifier, row, col, identifier);
  }

  std::stringstream ss;
  ss << "Unhandled character '" << ch << "'";
  return Result<Token>::Error(ss.str());
}

}  // namespace xos
