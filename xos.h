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

namespace xos {

class Token {
 public:
  enum Kind {
    eof,
    out,
    lparen,
    rparen,
    arrow,
    string,
    identifier,
    number,
  };

  Token(Kind kind, uint32_t row, uint32_t col, std::string val)
      : kind_(kind), row_(row), col_(col), val_(val) {}
  Token(Kind kind, uint32_t row, uint32_t col)
      : kind_(kind), row_(row), col_(col) {}
  Token() = default;

  Kind getKind() const { return kind_; }
  uint32_t getRow() const { return row_; }
  uint32_t getCol() const { return col_; }
  std::string getVal() const { return val_; }

  bool operator==(const Token &other) const {
    return kind_ == other.kind_ && row_ == other.row_ && col_ == other.col_ &&
           val_ == other.val_;
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
  Result(T &&result) : result_(std::move(result)) {}

  template <typename U>
  Result(const Result<U> &other) : err_(other.getErr()) {}

  class BuildError;
  Result(const BuildError &builder) : err_(builder.get()) {}

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
  T &get() {
    assert(!hasError() && "Getting from an invalid result");
    return result_;
  }
  const auto &getErr() const {
    assert(hasError() && "Expected an error");
    return err_;
  }

  // Example usage:
  //
  //   Result<Token> getTok() {
  //     ...
  //     if (err)
  //       return Result<Token>::BuildError() << "Error on row " << row;
  //     ...
  //   }
  //
  class BuildError {
   public:
    BuildError() {}

    template <typename U>
    BuildError &operator<<(const U &u) {
      buffer_ << u;
      return *this;
    }

    std::string get() const { return buffer_.str(); }

   private:
    std::stringstream buffer_;
  };

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

  std::istream &input_;
  uint32_t row_ = 1, col_ = 0;

  bool has_lookahead_ = false;
  int lookahead_;
};

namespace ast {

class Str {
 public:
  Str(const std::string &str) : str_(str) {}
  std::string getStr() const { return str_; }

  bool operator==(const Str &other) const { return str_ == other.str_; }

 private:
  std::string str_;
};

class Out {
 public:
  Out(std::unique_ptr<Str> &&expr) : expr_(std::move(expr)) {}
  const Str &getExpr() const { return *expr_; }

 private:
  std::unique_ptr<Str> expr_;
};

}  // namespace ast

class Parser {
 public:
  Parser(Lexer &lexer) : lexer_(lexer) {}
  Result<std::unique_ptr<ast::Str>> parseStr();
  std::unique_ptr<ast::Out> parseOut();

 private:
  Lexer &lexer_;
};

}  // namespace xos
