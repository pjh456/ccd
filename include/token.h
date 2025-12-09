#pragma once

#include <stdint.h>

typedef enum
{
    T_ID,
    T_NUM,

    T_PLUS,
    T_MINUS,
    T_STAR,
    T_DIV,

    T_EOF,
    T_UNKNOWN
} TokenType;

typedef struct
{
    TokenType type;
    const char *start;
    size_t length;
    int line;
    int col;
} Token;

const char *token_name(TokenType tt);
void print_token(const Token *t);
Token make_token(TokenType tt, const char *start, size_t len);