#include <token.h>

#include <stdio.h>

const char *token_name(TokenType tt)
{
    switch (tt)
    {
    case T_ID:
        return "ID";
    case T_NUM:
        return "NUM";
    case T_PLUS:
        return "PLUS";
    case T_MINUS:
        return "MINUS";
    case T_STAR:
        return "STAR";
    case T_DIV:
        return "DIV";
    case T_EOF:
        return "EOF";
    case T_UNKNOWN:
    default:
        return "UNKNOWN";
    }
}

void print_token(const Token *t)
{
    printf(
        "%s (%.*s) at %d:%d\n",
        token_name(t->type),
        (int)t->length, t->start,
        t->line, t->col);
}

Token make_token(TokenType tt, const char *start, size_t len)
{
    Token t;
    t.type = tt;
    t.start = start;
    t.length = len;
    return t;
}