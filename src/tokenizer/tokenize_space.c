#include "tokenizer.h"
#include "tokenizer_impl/token.h"
#include "tokenizer_impl/tokenizer_impl.h"
#include "utils.h"
#include <stdio.h>

// 循环跳过空白符和注释，直到遇到有效代码字符
void skip_space(Tokenizer *tk)
{
    while (is_space(peek(tk)) || consume_newline(tk) || peek(tk) == '/')
    {
        while (peek(tk) == '/')
        {
            char ch = (tk->pos + 1) < tk->len ? tk->src[tk->pos + 1] : '\0';
            if (ch == '/' || ch == '*')
                skip_comment(tk); // 进入注释处理
            else if (is_space(ch))
                advance(tk);
            else
                break;
        }
        if (is_space(peek(tk)))
            advance(tk);
        else if (consume_newline(tk))
            advance(tk);
    }
}

void skip_comment(Tokenizer *tk)
{
    advance(tk); // skip /
    if (peek(tk) == '*')
    {
        advance(tk); // skip *
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
        while (peek(tk) != '\0' && !consume_newline(tk))
            advance(tk);
        if (peek(tk) == '\n')
            advance(tk);
    }
}

Token *tokenize_preprocessor(Tokenizer *tk)
{
    Token *t = make_token(tk, T_PREPROCESS, NULL, 0);
    const char *const p = tk->src + tk->pos;

    advance(tk); // 跳过 '#'

    int connect_next_line = 0; // 是否有行连接符

    while (peek(tk) != '\0')
    {
        char ch = peek(tk);

        if (ch == '\\')
        {
            advance(tk);
            if (consume_newline(tk))
                connect_next_line = 1;
        }

        // 如果遇到真正的行结束 → token 结束
        if (consume_newline(tk))
        {
            if (!connect_next_line)
                break;
        }

        advance(tk);
    }

    size_t length = tk->src + tk->pos - p;
    while (length > 0 && is_space(*(p + length - 1)))
        length--;

    if (length)
        t->str = str_n_clone(p, length);
    else
        t->str = NULL;

    return t;
}

Token *tokenize_eof(Tokenizer *tk)
{
    Token *t = make_token(tk, T_EOF, NULL, 1);
    advance(tk);
    return t;
}

Token *tokenize_unknown(Tokenizer *tk)
{
    Token *t = make_token(tk, T_UNKNOWN, tk->src + tk->pos, 1);
    advance(tk); // 暂时这么写，之后会有贪婪匹配
    return t;
}