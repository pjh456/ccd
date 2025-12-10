#pragma once

#include "tokenizer_impl/token.h"
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

// 构造函数：初始化 Tokenizer
Tokenizer *tokenizer_new(const char *src);

// 析构函数：释放 Tokenizer 内存
void tokenizer_free(Tokenizer *tk);

// 偷看一眼：返回当前字符，但不移动光标
char peek(Tokenizer *tk);

// 步进：返回当前字符，并将光标向前移动一格
// 同时负责更新行号和列号
char advance(Tokenizer *tk);

// === 核心调度逻辑 ===
// 这是词法分析器的主循环入口
Token next(Tokenizer *tk);
