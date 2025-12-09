#pragma once

#include <tokenizer.h>
#include <ctype.h>

#define ONCE(block) \
    do              \
    {               \
        block       \
    } while (0)

int is_alpha(unsigned char ch);
int is_digit(unsigned char ch);
int is_alnum(unsigned char ch);
int is_space(unsigned char ch);

#ifdef ENABLE_DEBUG
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG(...)
#endif