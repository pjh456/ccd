#define ENABLE_DEBUG

#include "tokenizer.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SRC(T) ((T)->src)
#define POS(T) ((T)->pos)
#define LEN(T) ((T)->len)
#define CUR(T) (SRC(T) + POS(T))
#define CUR_CHAR(T) ((unsigned char)(*CUR(T)))

#define KEYWORD(T, pos, tt, str)                 \
    if (strncmp(pos, str, sizeof(str) - 1) == 0) \
    {                                            \
        POS(T) += sizeof(str) - 1;               \
        return make_token(tt, pos, sizeof(str)); \
    }

#define EAT_IF(T, ch) ONCE(if (CUR_CHAR(T) == (ch)) advance(T);)
#define EAT_WHILE(T, cond) ONCE(while (cond(CUR_CHAR(T))) advance(T);)

Tokenizer *tokenizer_new(const char *src)
{
    Tokenizer *tk = malloc(sizeof(*tk));
    SRC(tk) = src;
    POS(tk) = 0;
    LEN(tk) = strlen(src);
    return tk;
}

void tokenizer_free(Tokenizer *tk)
{
    free(tk);
}

char peek(Tokenizer *tk)
{
    return (POS(tk) < LEN(tk)) ? SRC(tk)[POS(tk)] : '\0';
}

char advance(Tokenizer *tk)
{
    if (POS(tk) < LEN(tk))
        return SRC(tk)[POS(tk)++];
    return '\0';
}

Token tokenizer_next(Tokenizer *tk)
{
    EAT_WHILE(tk, is_space);
    const char *start = CUR(tk);
    char ch = peek(tk);

    if (ch == '\0')
    {
        advance(tk);
        return make_token(T_EOF, start, 0);
    }

    if (is_alpha(ch))
    {
        advance(tk);
        EAT_WHILE(tk, is_alnum);
        return make_token(T_ID, start, CUR(tk) - start);
    }

    if (is_digit(ch))
    {
        advance(tk);
        EAT_WHILE(tk, is_digit);
        return make_token(T_NUM, start, CUR(tk) - start);
    }

    KEYWORD(tk, start, T_PLUS, "+");
    KEYWORD(tk, start, T_MINUS, "-");
    KEYWORD(tk, start, T_STAR, "*");
    KEYWORD(tk, start, T_ID, "/");

    DEBUG("Unexpected character '%c' at %d:%d\n", CUR_CHAR(tk), 0, 0);
    advance(tk);
    return make_token(T_UNKNOWN, start, 0);
}