#pragma once

#include "token.h"
#include <stdint.h>

typedef struct Status
{
    size_t line;
    size_t col;
} Status;

typedef struct Tokenizer
{
    const char *src;
    size_t pos;
    size_t len;
    Status stus;
} Tokenizer;

Tokenizer *tokenizer_new(const char *src);
void tokenizer_free(Tokenizer *tk);

char peek(Tokenizer *tk);
char advance(Tokenizer *tk);
Token next(Tokenizer *tk);
