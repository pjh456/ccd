#include "tokenizer.h"
#include "tokenizer_impl/tokenizer_impl.h"
#include "utils.h"

// 循环跳过空白符和注释，直到遇到有效代码字符
void skip_space(Tokenizer *tk)
{
    while (is_space(peek(tk)) || peek(tk) == '/')
    {
        if (peek(tk) == '/')
        {
            char ch = tk->src[tk->pos + 1];
            if (ch == '/' || ch == '*')
                skip_comment(tk); // 进入注释处理
        }
        advance(tk);
    }
}

void skip_comment(Tokenizer *tk)
{
    advance(tk); // skip /
    if (peek(tk) == '*')
    {
        while (peek(tk) != '\0')
        {
            if (peek(tk) == '*')
            {
                advance(tk);
                if (peek(tk) == '/')
                    break;
            }
            else
                advance(tk);
        }
    }
    else
    {
        while (peek(tk) != '\n')
            advance(tk);
    }
}

Token tokenize_preprocessor(Tokenizer *tk)
{
    Token t = make_token(tk, T_PREPROCESS, 0);
    int connect_next_line = 0;
    while (peek(tk) != '\0')
    {
        if (peek(tk) == '\\')
            connect_next_line = 1;
        if (peek(tk) == '\n')
        {
            if (connect_next_line)
                connect_next_line = 0;
            else
                break;
        }
        advance(tk);
    }
    t.length = tk->src + tk->pos - t.start;
    return t;
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