#ifndef XOS_H_
#define XOS_H_

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

// Just in case we ever decide to parse a file with more than 2^32 rows/cols,
// let's reserve the type here so we can change it easily.
using pos_t = uint32_t;

struct SourceLocation {
  pos_t row, col;

  bool operator==(const SourceLocation &other) const {
    return row == other.row && col == other.col;
  }
};

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

  Token(Kind kind, pos_t row, pos_t col, std::string val)
      : kind_(kind), start_{row, col}, val_(val) {}
  Token(Kind kind, pos_t row, pos_t col) : kind_(kind), start_{row, col} {}

  Kind getKind() const { return kind_; }
  pos_t getRow() const { return start_.row; }
  pos_t getCol() const { return start_.col; }
  std::string getVal() const { return val_; }

  bool operator==(const Token &other) const {
    return kind_ == other.kind_ && start_ == other.start_ && val_ == other.val_;
  }

 private:
  Kind kind_;
  SourceLocation start_;
  std::string val_;
};

template <typename T>
class Result {
 public:
  template <typename... ArgsTy>
  Result(ArgsTy... args) : result_(new T(std::forward<ArgsTy>(args)...)) {}
  Result(const T &result) : result_(new T(result)) {}
  Result(T &&result) : result_(new T(std::move(result))) {}

  template <typename U>
  Result(const Result<U> &other) : err_(other.getErr()) {}

  class BuildError;
  Result(const BuildError &builder) : err_(builder.get()) {}

  static Result Error(const std::string &msg) {
    Result result;
    result.err_ = msg;
    return result;
  }

  bool hasError() const { return !result_; }
  const T &get() const {
    assert(!hasError() && "Getting from an invalid result");
    return *result_;
  }
  T &get() {
    assert(!hasError() && "Getting from an invalid result");
    return *result_;
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
  std::unique_ptr<T> result_;
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
class Expr {
 public:
  virtual ~Expr() = default;
};

class Prototype {
 public:
  Prototype(const std::string &name, std::vector<std::string> args,
            std::string return_type)
      : name_(name), args_(args), return_type_(return_type) {}

 private:
  std::string name_;
  std::vector<std::string> args_;
  std::string return_type_;
};

class Func {
 public:
  Func(std::unique_ptr<Prototype> &&proto, std::unique_ptr<Expr> &&body)
      : proto_(std::move(proto)), body_(std::move(body)) {}

 private:
  std::unique_ptr<Prototype> proto_;
  std::unique_ptr<Expr> body_;
};

class Str : public Expr {
 public:
  Str(const std::string &str) : str_(str) {}
  std::string getStr() const { return str_; }

  bool operator==(const Str &other) const { return str_ == other.str_; }

 private:
  std::string str_;
};

class Out : public Expr {
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
  std::unique_ptr<ast::Func> parseFunc();

 private:
  Lexer &lexer_;
};

}  // namespace xos

#endif  // XOS_H_
