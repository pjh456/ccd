#include "utils.h"
#include <stdio.h>

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