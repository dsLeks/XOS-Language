#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>

namespace xos {

class Token {
 public:
  enum Kind {
    eof = -1,
    main = 1,
    out = 2,
    lparen = 3,
    rparen = 4,
    arrow = 5,
    string = 6,
    identifier = 7,
    number = 8,
  };

  Token(Kind kind, uint32_t row, uint32_t col)
      : kind_(kind), row_(row), col_(col) {}
  Token() = default;

  Kind getKind() const { return kind_; }
  uint32_t getRow() const { return row_; }
  uint32_t getCol() const { return col_; }

 private:
  Kind kind_;
  uint32_t row_, col_;
};

template <typename T>
class Result {
 public:
  template <typename... ArgsTy>
  Result(ArgsTy... args) : result_(std::forward<ArgsTy>(args)...) {}

  static Result Error(const std::string &msg) {
    Result result;
    result.err_ = msg;
    return result;
  }

  bool hasError() const { return !err_.empty(); }
  const T &get() const { return result_; }
  const auto &getErr() const { return err_; }

 private:
  Result() = default;

  std::string err_;
  T result_;
};

class Lexer {
 public:
  Lexer(const char *f);
  Result<Token> getNextToken();

 private:
  int getNextChar();

  Result<Token> makeResult(Token::Kind kind) {
    return Result<Token>(kind, row_, col_);
  }

  std::string IdentifierStr;
  double NumVal;
  FILE *fp;
  uint32_t row_ = 0, col_ = 0;

  bool has_lookahead_ = false;
  int lookahead_;
};

}  // namespace xos
