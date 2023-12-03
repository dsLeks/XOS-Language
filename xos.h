#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum Token
{
    tok_eof = -1,
    tok_main = -2,
    tok_out = -3,
    tok_identifier = -4,
    tok_number = -5,
}