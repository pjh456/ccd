#define ENABLE_DEBUG

#include "tokenizer.h"
#include "tokenizer_impl/tokenizer_impl.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SRC(T) ((T)->src)
#define POS(T) ((T)->pos)
#define LEN(T) ((T)->len)
#define STATUS(T) ((T)->stus)
#define CUR(T) (SRC(T) + POS(T))
#define CUR_CHAR(T) ((unsigned char)(*CUR(T)))
#define LINE(T) (STATUS(T).line)
#define COLUMN(T) (STATUS(T).col)
#define TOKENIZE_STATUS(T) (STATUS(T).stus)

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
    SRC(tk) = src, POS(tk) = 0, LEN(tk) = strlen(src);
    LINE(tk) = COLUMN(tk) = 1;
    return tk;
}

void tokenizer_free(Tokenizer *tk)
{
    if (!tk)
        return;
    free(tk);
}

char peek(Tokenizer *tk)
{
    return (POS(tk) < LEN(tk)) ? SRC(tk)[POS(tk)] : '\0';
}

char advance(Tokenizer *tk)
{
    if (peek(tk) == '\n')
        tk->stus.line++, tk->stus.col = 1;
    else
        tk->stus.col++;

    if (POS(tk) < LEN(tk))
        return SRC(tk)[POS(tk)++];
    return '\0';
}

Token next(Tokenizer *tk)
{
    skip_space(tk);
    char ch = peek(tk);
    if (ch == '\0')
        return tokenize_eof(tk);
    else if (is_alpha(ch))
        return tokenize_keyword(tk);
    else if (is_digit(ch))
        return tokenize_number(tk);

    switch (ch)
    {
    case '\'':
        return tokenize_char(tk);
    case '\"':
        return tokenize_string(tk);
    case '#':
        return tokenize_preprocessor(tk);
    case '<':
    case '>':
    case '=':
    case '(':
    case ')':
    case '[':
    case ']':
    case '{':
    case '}':
    case '!':
    case '&':
    case '|':
    case '^':
    case '~':
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case '\\':
    case '?':
    case ',':
    case '.':
    case ':':
    case ';':
        return tokenize_operator(tk);
    default:
        return tokenize_unknown(tk);
    }
}