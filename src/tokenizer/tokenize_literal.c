#include "tokenizer.h"
#include "tokenizer_impl/tokenizer_impl.h"
#include "utils.h"

#include <string.h>

typedef struct
{
    const char *kw;
    TokenType type;
} KwEntry;

static const KwEntry kw_table[] = {

    {"extern", T_EXTERN},
    {"static", T_STATIC},
    {"inline", T_INLINE},
    {"register", T_REGISTER},
    {"restrict", T_RESTRICT},
    {"volatile", T_VOLATILE},
    {"const", T_CONST},

    {"auto", T_AUTO},
    {"void", T_VOID},
    {"signed", T_SIGNED},
    {"unsigned", T_UNSIGNED},
    {"char", T_CHAR},
    {"short", T_SHORT},
    {"int", T_INT},
    {"long", T_LONG},
    {"float", T_FLOAT},
    {"double", T_DOUBLE},

    {"do", T_DO},
    {"while", T_WHILE},
    {"for", T_FOR},
    {"continue", T_CONTINUE},
    {"break", T_BREAK},
    {"if", T_IF},
    {"else", T_ELSE},
    {"switch", T_SWITCH},
    {"case", T_CASE},
    {"default", T_DEFAULT},
    {"return", T_RETURN},
    {"goto", T_GOTO},

    {"enum", T_ENUM},
    {"sizeof", T_SIZEOF},
    {"struct", T_STRUCT},
    {"typedef", T_TYPEDEF},
    {"union", T_UNION},

    {NULL, T_UNKNOWN},
};

Token tokenize_keyword(Tokenizer *tk)
{
    Token t = make_token(tk, T_UNKNOWN, 1);
    const char *p = t.start;

    for (const KwEntry *e = kw_table; e->kw; e++)
    {
        size_t len = strlen(e->kw);
        if (tk->pos + len <= tk->len &&
            strncmp(p, e->kw, len) == 0)
        {
            if (!is_alnum(p[len]))
            {
                t.type = e->type;
                t.length = len;
                tk->pos += len;
                tk->stus.col += len;
                return t;
            }
        }
    }

    advance(tk);
    t.type = T_IDENTIFIER;
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
    Token t = make_token(tk, T_CHARACTER, 0);
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