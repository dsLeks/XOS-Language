#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <istream>
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

  Token(Kind kind, uint32_t row, uint32_t col, std::string val)
      : kind_(kind), row_(row), col_(col), val_(val) {}
  Token() = default;

  Kind getKind() const { return kind_; }
  uint32_t getRow() const { return row_; }
  uint32_t getCol() const { return col_; }
  std::string getVal() const { return val_; }

  bool operator==(const Token &other) const {
    return kind_ == other.kind_ && row_ == other.row_ && col_ == other.col_;
  }

 private:
  Kind kind_;
  uint32_t row_, col_;
  std::string val_;
};

template <typename T>
class Result {
 public:
  template <typename... ArgsTy>
  Result(ArgsTy... args) : result_(std::forward<ArgsTy>(args)...) {}
  Result(const T &result) : result_(result) {}

  static Result Error(const std::string &msg) {
    Result result;
    result.err_ = msg;
    return result;
  }

  bool hasError() const { return !err_.empty(); }
  const T &get() const {
    assert(!hasError() && "Getting from an invalid result");
    return result_;
  }
  const auto &getErr() const { return err_; }

 private:
  Result() = default;

  std::string err_;
  T result_;
};

class Lexer {
 public:
  Lexer(std::istream &input);
  Result<Token> getNextToken();

 private:
  int getNextChar();

  std::string IdentifierStr;
  double NumVal;
  std::istream &input_;
  uint32_t row_ = 1, col_ = 0;

  bool has_lookahead_ = false;
  int lookahead_;
};

}  // namespace xos
