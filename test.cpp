#include <gtest/gtest.h>

#include "xos.h"

TEST(Lexer, Identifiers) {
  xos::Lexer lexer("test_input.txt");
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
