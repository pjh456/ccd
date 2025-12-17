# 09. 给单词办理身份证：Token 类型全定义与 Tokenizer 上下文

**本章目标**：

1.  **枚举爆炸**：将 `TokenType` 扩充到覆盖 C 语言的所有关键字和复杂运算符（如 `<<=`, `->`）。
2.  **结构定义**：正式定义 `Tokenizer` 上下文结构，记录行号、列号和源码位置。
3.  **基础设施**：实现 `tokenizer_new` 和 `tokenizer_free`，准备好词法分析的战场。

---

## 1. 扩充 TokenType：从玩具到正规军

之前的 `token.h` 只有几个简单的类型。现在我们需要支持完整的 C11 语法。
C 语言的运算符非常多，尤其是 **复合赋值**（如 `+=`）和 **位运算**（如 `<<`）。

> 为避免引入更复杂的实现，实际上是以 C98 标准实现的。

请修改 `include/token.h`（或 `include/tokenizer_impl/token.h`，取决于你的文件组织）：

```c
#pragma once

#include <stddef.h> // size_t

typedef enum {
    // === 基础类型 ===
    T_IDENTIFIER, // 变量名 (main, count)
    T_NUMBER,     // 数字 (123, 3.14)
    T_CHARACTER,  // 字符 ('a')
    T_STRING,     // 字符串 ("hello")

    // === 位运算符 ===
    T_NOT, T_TILDE, T_AND, T_OR, T_XOR, // ! ~ & | ^
    T_LEFT_SHIFT, T_RIGHT_SHIFT,        // << >>

    // === 算术运算符 ===
    T_PLUS, T_MINUS, T_STAR, T_DIV, T_MOD, // + - * / %
    T_ASSIGN, // =

    // === 比较/逻辑运算符 ===
    T_LESS, T_GREATER, T_EQUAL, T_NOT_EQUAL,      // < > == !=
    T_LESS_EQUAL, T_GREATER_EQUAL,                // <= >=
    T_AND_AND, T_OR_OR,                           // && ||

    // === 复合赋值 (这里很容易漏！) ===
    T_AND_ASSIGN, T_OR_ASSIGN, T_XOR_ASSIGN,      // &= |= ^=
    T_LEFT_SHIFT_ASSIGN, T_RIGHT_SHIFT_ASSIGN,    // <<= >>=
    T_PLUS_ASSIGN, T_MINUS_ASSIGN,                // += -=
    T_MUL_ASSIGN, T_DIV_ASSIGN, T_MOD_ASSIGN,     // *= /= %=

    // === 自增自减 ===
    T_INC, T_DEC, // ++ --

    // === 特殊符号 ===
    T_PREPROCESS, // #
    T_BACKSLASH,  // \

    // === 分隔符 ===
    T_LEFT_PAREN, T_RIGHT_PAREN,     // ( )
    T_LEFT_BRACKET, T_RIGHT_BRACKET, // [ ]
    T_LEFT_BRACE, T_RIGHT_BRACE,     // { }
    T_COMMA, T_COLON, T_SEMICOLON,   // , : ;
    T_DOT, T_ARROW, T_QUESTION,      // . -> ?
    T_ELLIPSIS,                      // ...

    // === C 语言关键字 (C11) ===
    T_EXTERN, T_STATIC, T_CONST, T_VOID, T_INT, T_CHAR, // ... 省略部分，请参照源码补全
    T_IF, T_ELSE, T_WHILE, T_FOR, T_RETURN, T_STRUCT, 
    // ... 务必把所有关键字补全，否则 parser 阶段会把你当作变量名处理

    // === 状态控制 ===
    T_EOF,    // 文件结束
    T_UNKNOWN // 无法识别的垃圾字符
} TokenType;

typedef struct {
    TokenType type;
    char *str;      // Token 内容 (深拷贝)
    int line;       // 行号
    int col;        // 列号
} Token;

// 调试辅助：把枚举数字转成字符串 "T_INT"
const char *token_name(TokenType tt);
void print_token(const Token *t);
void token_free(Token *t);
```

> **注意**：完整的枚举列表非常长，建议直接复制项目源码中的实现。

---

## 2. Tokenizer 上下文：保存扫描状态

`Token` 是产物，而 `Tokenizer` 是工厂。我们需要一个结构体来保存“当前扫描到哪里了”。

在 `include/tokenizer.h` 中定义：

```c
#pragma once
#include "token.h"

// 记录行号和列号的小助手
typedef struct {
    size_t line;
    size_t col;
} Status;

typedef struct {
    const char *src; // 源代码字符串 (只读引用，不拥有所有权)
    size_t pos;      // 当前解析到的字符索引 (Cursor)
    size_t len;      // 源代码总长度
    Status stus;     // 当前的行号/列号状态
} Tokenizer;

// --- 核心接口 ---

// 1. 创建与销毁
Tokenizer *tokenizer_new(const char *src);
void tokenizer_free(Tokenizer *tk);

// 2. 核心循环：一次吐出一个 Token
Token *next(Tokenizer *tk);

// 3. 一次性处理：直接把整个字符串变成 Token 向量
// 这是给外部调用的主入口
struct Vector *tokenize_all(const char *src);
```

---

## 3. 实现 Tokenizer 基础设施

在 `src/tokenizer.c` 中，我们先实现初始化和销毁逻辑，以及最基础的 **“光标移动”** 函数。

```c
#include "tokenizer.h"
#include <stdlib.h>
#include <string.h>

// 构造函数
Tokenizer *tokenizer_new(const char *src) {
    Tokenizer *tk = malloc(sizeof(*tk));
    tk->src = src;
    tk->pos = 0;
    tk->len = strlen(src); // 计算长度
    
    // 初始化状态：行号列号从 1 开始
    tk->stus.line = 1;
    tk->stus.col = 1;
    
    return tk;
}

// 析构函数
void tokenizer_free(Tokenizer *tk) {
    if (!tk) return;
    // 注意：tk->src 是外部传入的字符串（通常来自 utils.c 的 read_file），
    // Tokenizer 不负责释放它，谁申请谁释放。
    free(tk); 
}

// === 核心辅助函数 ===

// 1. Peek (偷看)：返回当前字符，但不移动光标
// 如果到了末尾，返回 \0
char peek(Tokenizer *tk) {
    if (tk->pos >= tk->len) return '\0';
    return tk->src[tk->pos];
}

// 2. Advance (步进)：返回当前字符，并移动光标
// 关键职责：负责维护 line 和 col
char advance(Tokenizer *tk) {
    char ch = peek(tk);
    
    // 处理换行符：行号+1，列号归1
    // 注意：Windows 的 \r\n 问题通常在 read_file 阶段解决，
    // 或者在这里增加对 \r 的特殊判断
    if (ch == '\n') {
        tk->stus.line++;
        tk->stus.col = 1;
    } else {
        tk->stus.col++;
    }
    
    // 移动光标
    if (tk->pos < tk->len) {
        tk->pos++;
    }
    
    return ch;
}
```

---

## 4. 实现调试辅助函数

在 `src/token.c` 中，实现 `token_name`。这虽然是个巨大的 `switch-case`，但对于调试必不可少。

```c
const char *token_name(TokenType tt) {
    switch (tt) {
        case T_IDENTIFIER: return "IDENTIFIER";
        case T_NUMBER:     return "NUMBER";
        case T_IF:         return "IF";
        case T_WHILE:      return "WHILE";
        // ... (此处省略 100 行代码，请把所有枚举都写上) ...
        case T_EOF:        return "EOF";
        default:           return "UNKNOWN";
    }
}
```

---

## 5. 更新 CMake

别忘了把新文件加入构建系统：

```cmake
set(SOURCES
    src/main.c
    src/vector.c
    src/utils.c
    src/token.c       # Token 结构体相关
    src/tokenizer.c   # Tokenizer 逻辑相关
)
```

---

### 下一章预告

基础设施已经搭建完毕。我们有了 `Tokenizer` 对象，也有了 `peek` 和 `advance` 这两个“眼睛”和“腿”。

下一章 **[10. 词法分析器实现：扫描循环](10_tokenizer_loop.md)**，我们将实现最核心的调度逻辑 `next()`：

```c
Token *next(Tokenizer *tk) {
    // 1. 跳过空格
    // 2. if (is_digit) -> 解析数字
    // 3. if (is_alpha) -> 解析关键字
    // 4. switch (char) -> 解析符号
}
```

这是整个词法分析器的心脏。