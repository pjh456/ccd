#include "tokenizer.h"
#include "tokenizer_impl.h"
#include <string.h>

typedef struct
{
    const char *op;
    TokenType type;
} OpEntry;

static const OpEntry op_table[] = {

    // ===== 长度 3 =====
    {"<<=", T_LEFT_SHIFT_ASSIGN},
    {">>=", T_RIGHT_SHIFT_ASSIGN},

    // ===== 长度 2 =====
    {"->", T_ARROW},
    {"==", T_EQUAL},
    {"!=", T_NOT_EQUAL},
    {"<=", T_LESS_EQUAL},
    {">=", T_GREATER_EQUAL},

    {"&&", T_AND_AND},
    {"||", T_OR_OR},

    {"+=", T_PLUS_ASSIGN},
    {"-=", T_MINUS_ASSIGN},
    {"*=", T_MUL_ASSIGN},
    {"/=", T_DIV_ASSIGN},
    {"%=", T_MOD_ASSIGN},

    {"&=", T_AND_ASSIGN},
    {"|=", T_OR_ASSIGN},
    {"^=", T_XOR_ASSIGN},

    {"<<", T_LEFT_SHIFT},
    {">>", T_RIGHT_SHIFT},

    {"++", T_INC},
    {"--", T_DEC},

    // ===== 单字符运算符 =====
    {"!", T_NOT},
    {"~", T_TILDE},
    {"&", T_AND},
    {"|", T_OR},
    {"^", T_XOR},

    {"+", T_PLUS},
    {"-", T_MINUS},
    {"*", T_STAR},
    {"/", T_DIV},
    {"%", T_MOD},
    {"=", T_ASSIGN},

    {"<", T_LESS},
    {">", T_GREATER},

    {"#", T_PREPROCESS},
    {"\\", T_BACKSLASH},

    {"(", T_LEFT_PAREN},
    {")", T_RIGHT_PAREN},
    {"[", T_LEFT_BRACKET},
    {"]", T_RIGHT_BRACKET},
    {"{", T_LEFT_BRACE},
    {"}", T_RIGHT_BRACE},

    {",", T_COMMA},
    {":", T_COLON},
    {";", T_SEMICOLON},
    {".", T_DOT},
    {"?", T_QUESTION},

    {NULL, T_UNKNOWN},
};

Token tokenize_operator(Tokenizer *tk)
{
    Token t = make_token(tk, T_UNKNOWN, 1);
    const char *p = t.start;

    const OpEntry *best = NULL;
    size_t best_len = 0;

    for (const OpEntry *e = op_table; e->op; e++)
    {
        size_t len = strlen(e->op);
        if (tk->pos + len <= tk->len &&
            strncmp(p, e->op, len) == 0)
        {
            if (len > best_len)
            {
                best = e;
                best_len = len;
            }
        }
    }

    if (best)
    {
        t.type = best->type;
        t.length = best_len;
        tk->pos += best_len;
        tk->stus.col += best_len;
    }
    else
        advance(tk);
    return t;
}