#include <gtest/gtest.h>

#include "xos.h"

TEST(Lexer, Identifiers) {
  Lexer lexer("test_input.txt");
  ASSERT_EQ(lexer.get_next_token(), tok_main);
  ASSERT_EQ(lexer.get_next_token(), tok_lparen);
  ASSERT_EQ(lexer.get_next_token(), tok_lparen);
  ASSERT_EQ(lexer.get_next_token(), tok_rparen);
  ASSERT_EQ(lexer.get_next_token(), tok_arrow);
  ASSERT_EQ(lexer.get_next_token(), tok_lparen);
  ASSERT_EQ(lexer.get_next_token(), tok_rparen);
  ASSERT_EQ(lexer.get_next_token(), tok_rparen);
  ASSERT_EQ(lexer.get_next_token(), tok_out);
  ASSERT_EQ(lexer.get_next_token(), tok_string);
  ASSERT_EQ(lexer.get_next_token(), tok_eof);
}
