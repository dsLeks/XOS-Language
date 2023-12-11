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

  ASSERT_EQ(lexer.getNextToken().get(),
            xos::Token(xos::Token::identifier, 1, 1, "main"));
  ASSERT_EQ(lexer.getNextToken().get(), xos::Token(xos::Token::lparen, 1, 5));
  ASSERT_EQ(lexer.getNextToken().get(), xos::Token(xos::Token::lparen, 1, 7));
  ASSERT_EQ(lexer.getNextToken().get(), xos::Token(xos::Token::rparen, 1, 8));
  ASSERT_EQ(lexer.getNextToken().get(), xos::Token(xos::Token::arrow, 1, 10));
  ASSERT_EQ(lexer.getNextToken().get(), xos::Token(xos::Token::lparen, 1, 13));
  ASSERT_EQ(lexer.getNextToken().get(), xos::Token(xos::Token::rparen, 1, 14));
  ASSERT_EQ(lexer.getNextToken().get(), xos::Token(xos::Token::rparen, 1, 16));
  ASSERT_EQ(lexer.getNextToken().get(), xos::Token(xos::Token::out, 2, 3));
  ASSERT_EQ(lexer.getNextToken().get(),
            xos::Token(xos::Token::string, 2, 7, "Hello World"));
  ASSERT_EQ(lexer.getNextToken().get(), xos::Token(xos::Token::eof, 2, 20));
}

TEST(Lexer, UnfinishedArrowToken) {
  std::stringstream ss("=");
  xos::Lexer lexer(ss);

  auto result = lexer.getNextToken();
  ASSERT_TRUE(result.hasError());
  ASSERT_EQ(result.getErr(), "Expected '=>' at row 1, col 1");
}

TEST(Lexer, UnhandledCharacter) {
  std::stringstream ss("%");
  xos::Lexer lexer(ss);

  auto result = lexer.getNextToken();
  ASSERT_TRUE(result.hasError());
  ASSERT_EQ(result.getErr(), "Unhandled character '%'");
}

TEST(Parser, ParseStr) {
  std::stringstream ss("\"Hello World\"");
  xos::Lexer lexer(ss);
  xos::Parser parser(lexer);

  auto result = parser.parseStr();
  ASSERT_NE(result.get(), nullptr);
  ASSERT_EQ(result.get()->getStr(), "Hello World");
}

TEST(Parser, ParseStrError) {
  std::stringstream ss("Hello World");
  xos::Lexer lexer(ss);
  xos::Parser parser(lexer);

  auto result = parser.parseStr();
  ASSERT_TRUE(result.hasError());
  ASSERT_EQ(result.getErr(), "Expected string on row 1, col 1");
}

TEST(Parser, ParseOut) {
  std::stringstream ss("out \"Hello World\"");
  xos::Lexer lexer(ss);
  xos::Parser parser(lexer);

  auto result = parser.parseOut();
  ASSERT_NE(result.get(), nullptr);
  ASSERT_EQ(result->getExpr(), xos::ast::Str("Hello World"));
}

TEST(Result, ErrorBuilder) {
  std::string msg("msg");
  xos::Result<int> res = xos::Result<int>::BuildError()
                         << "Test error " << 1 << " " << msg;
  ASSERT_TRUE(res.hasError());
  ASSERT_EQ(res.getErr(), "Test error 1 msg");
}

}  // namespace
