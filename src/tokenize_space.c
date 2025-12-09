#include "tokenizer_impl.h"
#include "tokenizer.h"
#include "utils.h"

void skip_space(Tokenizer *tk)
{
    while (is_space(peek(tk)))
        advance(tk);
}

Token tokenize_eof(Tokenizer *tk)
{
    Token t = make_token(tk, T_EOF, 1);
    advance(tk);
    return t;
}

Token tokenize_unknown(Tokenizer *tk)
{
    Token t = make_token(tk, T_UNKNOWN, 1);
    advance(tk); // 暂时这么写，之后会有贪婪匹配
    return t;
}