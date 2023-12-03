#include "xos.h"
#include <string>
#include <stdlib.h>
#include <ctype.h>

class Lexer
{
    static std::string IdentifierStr;
    static double NumVal;

public:
    static int get_next_token()
    {
        static int last_char = ' ';

        // skip the whitespace
        while (isspace(last_char))
        {
            last_char = getchar();
        }

        if (isalpha(last_char))
        {
            IdentifierStr = last_char;
            while (isalnum((last_char = getchar())))
            {
                IdentifierStr += last_char;
            }
            if (IdentifierStr == "main")
                return tok_main;

            if (IdentifierStr == "out")
                return tok_out;
        }
    }
};