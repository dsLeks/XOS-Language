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

  ASSERT_EQ(lexer.getNextToken().get(), xos::Token(xos::Token::main, 1, 1));
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

}  // namespace
