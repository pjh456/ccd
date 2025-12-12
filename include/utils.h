#pragma once

#include "tokenizer.h"
#include <ctype.h>

// === 字符判断函数的封装 ===
// 这些函数封装了 ctype.h，防止传入 signed char 导致的未定义行为，并统一接口
int is_alpha(unsigned char ch);
int is_digit(unsigned char ch);
int is_alnum(unsigned char ch);
int is_space(unsigned char ch);

int consume_newline(Tokenizer *tk);

void print_indent(int indent);

char *str_clone(const char *str);

#ifdef ENABLE_DEBUG
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG(...)
#endif