#include <ctype.h>
#include <stdlib.h>

#include <iostream>
#include <string>

#include "xos.h"

Lexer::Lexer(const char *f) { fp = fopen(f, "r"); };

int Lexer::get_next_token() {
  char ch = getc(fp);

  // skip the whitespace
  while (isspace(ch)) {
    ch = getc(fp);
  }

  if (ch == EOF) return tok_eof;

  if (ch == '=') {
    ch = getc(fp);
    if (ch == '>') {
      return tok_arrow;
    } else {
      ungetc(ch, fp);
      return '9999999';
    }
  }
  if (ch == '(') {
    return tok_lparen;
  }
  if (ch == ')') {
    return tok_rparen;
  }

  if (ch == '"') {
    IdentifierStr = "";
    ch = getc(fp);
    while (ch != '"') {
      IdentifierStr += ch;
      ch = getc(fp);
    }
    return tok_string;
  }

  if (((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) && ch != EOF) {
    IdentifierStr = "";
    while (((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) &&
           ch != EOF) {
      IdentifierStr += ch;
      ch = getc(fp);
    }
    // if (ch != EOF)
    ungetc(ch, fp);
    if (IdentifierStr == "main") {
      return tok_main;
    }

    if (IdentifierStr == "out") {
      return tok_out;
    }
  }

  return tok_identifier;
}
