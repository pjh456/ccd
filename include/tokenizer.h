#pragma once
#include <token.h>

typedef struct Tokenizer
{
    const char *src;
    size_t pos;
    size_t len;
} Tokenizer;

Tokenizer *tokenizer_new(const char *src);
void tokenizer_free(Tokenizer *tk);

char peek(Tokenizer *tk);
char advance(Tokenizer *tk);

Token tokenizer_next(Tokenizer *tk);
