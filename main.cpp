#include <stdlib.h>
#include <string>
#include <iostream>
#include "xos.h"

int main()
{
    Lexer lexer("test_input.txt");
    while (1)
    {
        int tok = lexer.get_next_token();
        if (tok == tok_eof)
        {
            std::cout << tok << " , ";
            break;
        }
        std::cout << tok << " , ";
    }

    std::cout << std::endl;
    return 0;
}