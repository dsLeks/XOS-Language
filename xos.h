#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <istream>
#include <string>
#include <vector>
#include <memory>

namespace xos
{

  class Token
  {
  public:
    enum Kind
    {
      eof = -1,
      main = 1,
      out = 2,
      lparen = 3,
      rparen = 4,
      arrow = 5,
      tstring = 6,
      identifier = 7,
      number = 8,
    };

    /** The token can be a character, string, a number, or anything else.
     * -- Switch to using variant instead of union?
     */
    union val
    {
      char cval;
      const char *sval;
      int inum;
      bool eof;
      void *any;
    } v;

    Token(Kind kind, uint32_t row, uint32_t col, val v)
        : kind_(kind), row_(row), col_(col), v_(v) {}
    Token() = default;

    Kind getKind() const { return kind_; }
    uint32_t getRow() const { return row_; }
    uint32_t getCol() const { return col_; }
    val getVal() const { return v_; }

  private:
    Kind kind_;
    uint32_t row_, col_;
    val v_;
  };

  template <typename T>
  class Result
  {
  public:
    template <typename... ArgsTy>
    Result(ArgsTy... args) : result_(std::forward<ArgsTy>(args)...) {}

    static Result Error(const std::string &msg) // Why did you make this static?
    {
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

  class Lexer
  {
  public:
    Lexer(std::istream &input); // Should this not be ifstream?
    Result<Token> getNextToken();

  private:
    int getNextChar();

    Result<Token> makeResult(Token::Kind kind, Token::val v)
    {
      return Result<Token>(kind, row_, col_, v);
    }

    std::string IdentifierStr;
    double NumVal;
    std::istream &input_;
    uint32_t row_ = 0, col_ = 0;

    bool has_lookahead_ = false;
    int lookahead_;
  };

  class StrAST
  {
  public:
    std::string StrVal;

    StrAST(const std::string &StrVal) : StrVal(StrVal) {}
  };

  class PrototypeAST
  {
  public:
    std::string Name;
    std::vector<std::string> Args;
    std::string returnType;
    PrototypeAST(const std::string &Name, std::vector<std::string> Args, std::string returnType)
        : Name(Name), Args(std::move(Args)), returnType(std::move(returnType)) {}

    const std::string &getName() const { return Name; }
  };

  class FunctionAST
  {
  public:
    std::unique_ptr<PrototypeAST> Proto;
    std::unique_ptr<StrAST> Body;
    FunctionAST(std::unique_ptr<PrototypeAST> Proto,
                std::unique_ptr<StrAST> Body)
        : Proto(std::move(Proto)), Body(std::move(Body)) {}
  };

  class Parser
  {
  public:
    Lexer lexer_; // [TODO] make this unique_ptr?
    Parser(std::istream &input) : lexer_(input) {}
    void parse();
    std::unique_ptr<PrototypeAST> parsePrototype(xos::Result<xos::Token> tokIden);
    std::unique_ptr<FunctionAST> parseFunction(std::unique_ptr<PrototypeAST> proto);
  };

} // namespace xos
