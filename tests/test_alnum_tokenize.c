#include "tokenizer.h"

int main()
{
    const char src[] = "1+2*3";
    Tokenizer *tk = tokenizer_new(src);
    Token t = tokenizer_next(tk);
    while (t.type != T_EOF)
    {
        print_token(&t);
        t = tokenizer_next(tk);
    }
}