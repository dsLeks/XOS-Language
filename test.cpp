#include <gtest/gtest.h>

#include <sstream>

#include "xos.h"

namespace {

constexpr char kHelloWorldStr[] =
    "main( () => () )\n"
    "  out \"Hello World\"";

TEST(Lexer, HelloWorld) {
  std::stringstream ss(kHelloWorldStr);
  xos::Lexer lexer(ss);

  ASSERT_EQ(lexer.Lex().get(),
            xos::Token(xos::Token::identifier, 1, 1, "main"));
  ASSERT_EQ(lexer.Lex().get(), xos::Token(xos::Token::lparen, 1, 5));
  ASSERT_EQ(lexer.Lex().get(), xos::Token(xos::Token::lparen, 1, 7));
  ASSERT_EQ(lexer.Lex().get(), xos::Token(xos::Token::rparen, 1, 8));
  ASSERT_EQ(lexer.Lex().get(), xos::Token(xos::Token::arrow, 1, 10));
  ASSERT_EQ(lexer.Lex().get(), xos::Token(xos::Token::lparen, 1, 13));
  ASSERT_EQ(lexer.Lex().get(), xos::Token(xos::Token::rparen, 1, 14));
  ASSERT_EQ(lexer.Lex().get(), xos::Token(xos::Token::rparen, 1, 16));
  ASSERT_EQ(lexer.Lex().get(), xos::Token(xos::Token::out, 2, 3));
  ASSERT_EQ(lexer.Lex().get(),
            xos::Token(xos::Token::string, 2, 7, "Hello World"));
  ASSERT_EQ(lexer.Lex().get(), xos::Token(xos::Token::eof, 2, 20));
}

TEST(Lexer, ExpectEndlineCharacterAtLastChar) {
  char helloWorldStr[] = "\"Hello World\\n\"";
  std::stringstream ss(helloWorldStr);
  xos::Lexer lexer(ss);
  std::string expectedString = "Hello World";
  expectedString.push_back('\n');
  ASSERT_EQ(lexer.Lex().get(),
            xos::Token(xos::Token::string, 1, 1, expectedString));
}

TEST(Lexer, ExpectEndlineCharacterAtFirstChar) {
  char helloWorldStr[] = "\"\\nHello World\"";
  std::stringstream ss(helloWorldStr);
  xos::Lexer lexer(ss);
  std::string expectedString = "Hello World";
  expectedString.insert(0, 1, '\n');
  ASSERT_EQ(lexer.Lex().get(),
            xos::Token(xos::Token::string, 1, 1, expectedString));
}

TEST(Lexer, ExpectEndlineCharacterAtSixthChar) {
  char helloWorldStr[] = "\"Hello\\n World\"";
  std::stringstream ss(helloWorldStr);
  xos::Lexer lexer(ss);
  std::string expectedString = "Hello World";
  expectedString.insert(5, 1, '\n');
  ASSERT_EQ(lexer.Lex().get(),
            xos::Token(xos::Token::string, 1, 1, expectedString));
}

TEST(Lexer, Peek) {
  std::stringstream ss(kHelloWorldStr);
  xos::Lexer lexer(ss);

  xos::Token expected(xos::Token::identifier, 1, 1, "main");

  // We can peek as much as we want and see the same token.
  ASSERT_EQ(lexer.Peek().get(), expected);
  ASSERT_EQ(lexer.Peek().get(), expected);
  ASSERT_EQ(lexer.Peek().get(), expected);
  ASSERT_EQ(lexer.Lex().get(), expected);

  xos::Token lparen(xos::Token::lparen, 1, 5);
  ASSERT_EQ(lexer.Peek().get(), lparen);
  ASSERT_EQ(lexer.Peek().get(), lparen);
  ASSERT_EQ(lexer.Peek().get(), lparen);
}

TEST(Lexer, UnfinishedArrowToken) {
  std::stringstream ss("=");
  xos::Lexer lexer(ss);

  auto result = lexer.Lex();
  ASSERT_TRUE(result.hasError());
  ASSERT_EQ(result.getError(), "Expected '=>' at row 1, col 1");
}

TEST(Lexer, UnhandledCharacter) {
  std::stringstream ss("%");
  xos::Lexer lexer(ss);

  auto result = lexer.Lex();
  ASSERT_TRUE(result.hasError());
  ASSERT_EQ(result.getError(), "Unhandled character '%'");
}

TEST(Parser, ParseStr) {
  std::stringstream ss("\"Hello World\"");
  xos::Lexer lexer(ss);
  xos::Parser parser(lexer);

  auto result = parser.parseStr();
  ASSERT_FALSE(result.hasError());
  ASSERT_EQ(result.get().getStr(), "Hello World");
}

TEST(Parser, ParseStrError) {
  std::stringstream ss("Hello World");
  xos::Lexer lexer(ss);
  xos::Parser parser(lexer);

  auto result = parser.parseStr();
  ASSERT_TRUE(result.hasError());
  ASSERT_EQ(result.getError(), "Expected string on row 1, col 1");
}

TEST(Parser, ParseOut) {
  std::stringstream ss("out \"Hello World\"");
  xos::Lexer lexer(ss);
  xos::Parser parser(lexer);

  auto result = parser.parseOut();
  ASSERT_FALSE(result.hasError());
  ASSERT_EQ(result.get().getExpr(), xos::ast::Str("Hello World"));
}

TEST(Parser, ParseFunc) {
  std::stringstream ss("main( () => () ) out \"Hello World\"");
  xos::Lexer lexer(ss);
  xos::Parser parser(lexer);

  auto result = parser.parseFunc();
  ASSERT_FALSE(result.hasError()) << result.getError();

  xos::ast::Prototype proto("main", std::vector<std::string>(), "");
  std::unique_ptr<xos::ast::Str> hw(new xos::ast::Str("Hello World"));
  std::unique_ptr<xos::ast::Out> out(
      std::make_unique<xos::ast::Out>(std::move(hw)));
  xos::ast::Func f(std::make_unique<xos::ast::Prototype>(proto),
                   std::move(out));
  ASSERT_EQ(result.get(), f);
}

TEST(Result, ErrorBuilder) {
  std::string msg("msg");
  xos::Result<int> res = xos::Result<int>::BuildError()
                         << "Test error " << 1 << " " << msg;
  ASSERT_TRUE(res.hasError());
  ASSERT_EQ(res.getError(), "Test error 1 msg");
}

TEST(Result, IndependentStorageSize) {
  constexpr size_t kLargeSize = 1024;
  using large_t = char[kLargeSize];

  auto err = xos::Result<large_t>::Error("err");
  static_assert(
      sizeof(err) < sizeof(large_t),
      "The size of Result<T> should be independent from the size of T.");
}

TEST(Result, ResultDoesntRequireDefaultCtor) {
  struct S {
    S() = delete;
    int x, y;
  };

  [[maybe_unused]] auto err = xos::Result<S>::Error("err");
}

TEST(Result, OperatorOverloads) {
  struct S {
    int x;
    S(int x) : x(x) {}
    bool operator==(const S &other) const { return x == other.x; }
  };
  xos::Result<S> good_res(4);
  ASSERT_TRUE(good_res);
  ASSERT_EQ(*good_res, S{4});
  ASSERT_EQ(good_res->x, 4);

  auto bad_res = xos::Result<S>::Error("err msg");
  ASSERT_FALSE(bad_res);
}

TEST(Result, ErrornessIndependentOfValue) {
  // A result should be independent of the underlying value of the object. That
  // is, if the underlying value can have some "falseness", the Result of that
  // object should be treated as not an error unless it was constructed as an
  // error.
  xos::Result<int> int_res(0);
  xos::Result<int *> int_ptr_res(nullptr);

  struct S {
    operator bool() const { return false; }
  };
  xos::Result<S> s_res(S{});

  ASSERT_FALSE(int_res.hasError());
  ASSERT_FALSE(int_ptr_res.hasError());
  ASSERT_FALSE(s_res.hasError());

  auto int_err = xos::Result<int>::Error("");
  auto int_ptr_err = xos::Result<int *>::Error("");
  auto s_err = xos::Result<S>::Error("");

  ASSERT_TRUE(int_err.hasError());
  ASSERT_TRUE(int_ptr_err.hasError());
  ASSERT_TRUE(s_err.hasError());
}

}  // namespace
