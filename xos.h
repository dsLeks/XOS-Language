#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>

enum Token
{
    tok_eof = -1,
    tok_main = 1,
    tok_out = 2,
    tok_lparen = 3,
    tok_rparen = 4,
    tok_arrow = 5,
    tok_string = 6,
    tok_identifier = 7,
    tok_number = 8,
};

class Lexer
{
public:
    std::string IdentifierStr;
    double NumVal;
    FILE *fp;
    Lexer(const char *f);
    int get_next_token();
};
