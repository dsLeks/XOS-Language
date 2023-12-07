#include <gtest/gtest.h>

#include <sstream>

#include "xos.h"

TEST(Lexer, Identifiers) {
  std::stringstream ss;
  ss << "main( () => () )\n"
        "  out \"Hello World\"";
  xos::Lexer lexer(ss);

  ASSERT_EQ(lexer.getNextToken().get().getKind(), xos::Token::main);
  ASSERT_EQ(lexer.getNextToken().get().getKind(), xos::Token::lparen);
  ASSERT_EQ(lexer.getNextToken().get().getKind(), xos::Token::lparen);
  ASSERT_EQ(lexer.getNextToken().get().getKind(), xos::Token::rparen);
  ASSERT_EQ(lexer.getNextToken().get().getKind(), xos::Token::arrow);
  ASSERT_EQ(lexer.getNextToken().get().getKind(), xos::Token::lparen);
  ASSERT_EQ(lexer.getNextToken().get().getKind(), xos::Token::rparen);
  ASSERT_EQ(lexer.getNextToken().get().getKind(), xos::Token::rparen);
  ASSERT_EQ(lexer.getNextToken().get().getKind(), xos::Token::out);
  ASSERT_EQ(lexer.getNextToken().get().getKind(), xos::Token::string);
  ASSERT_EQ(lexer.getNextToken().get().getKind(), xos::Token::eof);
}
