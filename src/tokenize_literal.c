#include "tokenizer.h"
#include "tokenizer_impl.h"
#include "utils.h"

Token tokenize_identifier(Tokenizer *tk)
{
    Token t = make_token(tk, T_IDENTIFIER, 0);
    while (is_alnum(peek(tk)))
        advance(tk);
    t.length = tk->src + tk->pos - t.start;
    return t;
}

Token tokenize_number(Tokenizer *tk)
{
    Token t = make_token(tk, T_NUMBER, 0);
    while (is_digit(peek(tk)))
        advance(tk);

    if (peek(tk) == '.')
    {
        advance(tk);
        while (is_digit(peek(tk)))
            advance(tk);
    }

    t.length = tk->src + tk->pos - t.start;
    return t;
}

Token tokenize_char(Tokenizer *tk)
{
    Token t = make_token(tk, T_CHAR, 0);
    while (peek(tk) != '\'')
        advance(tk);
    advance(tk);
    t.length = tk->src + tk->pos - t.start;
    return t;
}

Token tokenize_string(Tokenizer *tk)
{
    Token t = make_token(tk, T_STRING, 0);
    while (peek(tk) != '\"')
        advance(tk);
    advance(tk);
    t.length = tk->src + tk->pos - t.start;
    return t;
}