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
  Result(const Result<U> &other) : err_(other.getError()) {}

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
  const auto &getError() const {
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
  Result<Token> Lex();

 private:
  int getNextChar();

  std::istream &input_;
  pos_t row_ = 1, col_ = 0;

  bool has_lookahead_ = false;
  int lookahead_;
};

// RTTI Helper Functions
//
// Classes can use this small version of RTTI by just implementing `static bool
// classof(const Node &) {...}`. The `classof` method is a static method used to
// determine if an object is an instance of the type we're checking.
//
// Example usage:
//
//   enum Kind { KA, KB };
//   struct A {
//     virtual Kind getKind() const { return KA; }
//   };
//   struct B : public A {
//     Kind getKind() const override { return KB; }
//     static bool classof(const A& a) { return a.getKind() == KB; }
//   };
//
//   void func(A *maybe_b_ptr) {
//     const B *b_ptr = dyn_cast<B>(maybe_b_ptr);
//     if (b_ptr) {
//       // b_ptr is definitely of type B.
//       ...
//     }
//     const B &b_ref = cast<B>(*maybe_b_ptr);
//     bool b = isa<B>(maybe_b_ptr);
//     ...
//   }
//
template <typename ToTy, typename FromTy>
const ToTy *dyn_cast(const FromTy &from) {
  if (ToTy::classof(from)) return static_cast<const ToTy *>(&from);
  return nullptr;
}

template <typename ToTy, typename FromTy>
const ToTy *dyn_cast(const FromTy *from) {
  if (ToTy::classof(*from)) return static_cast<const ToTy *>(from);
  return nullptr;
}

template <typename ToTy, typename FromTy>
bool isa(const FromTy *from) {
  return dyn_cast<ToTy>(from);
}

template <typename ToTy, typename FromTy>
bool isa(const FromTy &from) {
  return dyn_cast<ToTy>(from);
}

template <typename ToTy, typename FromTy>
const ToTy &cast(const FromTy &from) {
  assert(ToTy::classof(from));
  return static_cast<const ToTy &>(from);
}

template <typename ToTy, typename FromTy>
const ToTy *cast(const FromTy *from) {
  assert(ToTy::classof(*from));
  return static_cast<const ToTy *>(from);
}

namespace ast {

enum NodeKind {
  NK_Func,
  NK_Prototype,

  NK_ExprBegin,
  NK_Str = NK_ExprBegin,
  NK_Out,
  NK_ExprEnd,
};

class Node {
 public:
  virtual ~Node() = default;
  virtual NodeKind getKind() const = 0;
};

class Prototype : public Node {
 public:
  Prototype(const std::string &name, std::vector<std::string> args,
            std::string return_type)
      : name_(name), args_(args), return_type_(return_type) {}
  static bool classof(const Node &node) {
    return node.getKind() == NK_Prototype;
  }

  NodeKind getKind() const override { return NK_Prototype; }

 private:
  std::string name_;
  std::vector<std::string> args_;
  std::string return_type_;
};

class Expr;

class Func : public Node {
 public:
  Func(std::unique_ptr<Prototype> &&proto, std::unique_ptr<Expr> &&body)
      : proto_(std::move(proto)), body_(std::move(body)) {}
  static bool classof(const Node &node) { return node.getKind() == NK_Func; }

  NodeKind getKind() const override { return NK_Func; }

 private:
  std::unique_ptr<Prototype> proto_;
  std::unique_ptr<Expr> body_;
};

class Expr : public Node {
 public:
  static bool classof(const Node &node) {
    return NK_ExprBegin <= node.getKind() && node.getKind() < NK_ExprEnd;
  }
};

class Str : public Expr {
 public:
  Str(const std::string &str) : str_(str) {}
  static bool classof(const Node &node) { return node.getKind() == NK_Str; }

  NodeKind getKind() const override { return NK_Str; }

  std::string getStr() const { return str_; }

  bool operator==(const Str &other) const { return str_ == other.str_; }

 private:
  std::string str_;
};

class Out : public Expr {
 public:
  Out(std::unique_ptr<Str> &&expr) : expr_(std::move(expr)) {}
  static bool classof(const Node &node) { return node.getKind() == NK_Out; }

  NodeKind getKind() const override { return NK_Out; }

  const Str &getExpr() const { return *expr_; }

 private:
  std::unique_ptr<Str> expr_;
};

}  // namespace ast

class Parser {
 public:
  Parser(Lexer &lexer) : lexer_(lexer) {}
  Result<std::unique_ptr<ast::Str>> parseStr();
  Result<std::unique_ptr<ast::Out>> parseOut();
  Result<std::unique_ptr<ast::Func>> parseFunc();

 private:
  Lexer &lexer_;
};

}  // namespace xos

#endif  // XOS_H_
