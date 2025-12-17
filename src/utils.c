#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int is_alpha(unsigned char ch) { return isalpha(ch) || ch == '_'; }
int is_digit(unsigned char ch) { return ch >= '0' && ch <= '9'; }
int is_alnum(unsigned char ch) { return is_alpha(ch) || is_digit(ch); }
int is_space(unsigned char ch) { return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r'; }

int consume_newline(Tokenizer *tk)
{
    char ch = peek(tk);
    char nch = (tk->pos + 1 < tk->len) ? tk->src[tk->pos + 1] : '\0';

    if (ch == '\n')
        return 1;
    else if (ch == '\r')
    {
        if (nch == '\n')
            advance(tk);
        return 1;
    }

    return 0;
}

void print_indent(int indent)
{
    for (int i = 0; i < indent; i++)
        putchar(' ');
}

char *str_clone(const char *str)
{
    if (!str)
        return NULL;
    size_t len = strlen(str) + 1;
    char *p = malloc(len);
    memcpy(p, str, len);
    return p;
}

char *str_n_clone(const char *str, size_t n)
{
    if (!str || !n)
        return NULL;
    char *p = malloc(n + 1);
    memcpy(p, str, n);
    p[n] = '\0';
    return p;
}