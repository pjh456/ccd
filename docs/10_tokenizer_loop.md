# 10. 词法分析器实现：扫描循环与调度中心

**本章目标**：

1.  **架构落地**：理解 `Tokenizer` 的模块化拆分 (`tokenizer.c` vs `tokenize_*.c`)。
2.  **去除噪音**：实现 `skip_space`，处理最烦人的空格、换行和注释。
3.  **核心调度**：编写 `next()` 函数，它像一个交通指挥官，决定把当前字符交给谁处理。
4.  **驱动引擎**：实现 `tokenize_all`，一口气把文件读完。

---

## 1. 代码组织：分而治之

你可能注意到了，如果把所有代码都写在 `tokenizer.c` 里，那这个文件会膨胀到上千行。
为了保持代码整洁，我们将 Tokenizer 拆分为几个模块：

*   `src/tokenizer.c`: **调度中心**，负责初始化、销毁和主循环 `next()`。
*   `src/tokenize_space.c`: **清洁工**，负责跳过空白和注释。
*   `src/tokenize_literal.c`: **文员**，负责解析标识符、数字、字符串。
*   `src/tokenize_operator.c`: **数学家**，负责解析复杂的运算符。

我们需要一个内部头文件来共享这些子模块的函数声明。
请创建 `include/tokenizer_impl/tokenizer_impl.h` (或者直接放在 `src/` 下局部引用，取决于你的偏好，这里假设在 include 下)：

```c
#pragma once
#include "tokenizer.h"

// 这些函数只在 tokenizer 内部使用，不对外暴露
void skip_space(Tokenizer *tk);
void skip_comment(Tokenizer *tk);

Token *tokenize_keyword(Tokenizer *tk);
Token *tokenize_number(Tokenizer *tk);
Token *tokenize_char(Tokenizer *tk);
Token *tokenize_string(Tokenizer *tk);
Token *tokenize_preprocessor(Tokenizer *tk);
Token *tokenize_operator(Tokenizer *tk);
Token *tokenize_eof(Tokenizer *tk);
Token *tokenize_unknown(Tokenizer *tk);

// 辅助宏或函数
int is_alpha(char ch);
int is_digit(char ch);
int is_alnum(char ch);
int is_space(char ch);
```

---

## 2. 第一步：去除噪音 (Skip Space)

在解析具体代码前，必须把无意义的字符清理干净。
这比想象中复杂，因为还有 **注释**。

在 `src/tokenize_space.c` 中，我们实现了 `skip_space`：

```c
void skip_space(Tokenizer *tk) {
    // 只要是空白符，或者遇到换行，或者遇到除号 '/' (可能是注释)，就继续循环
    while (is_space(peek(tk)) || peek(tk) == '/') {
        
        // 特判：如果是 '/'，可能是除号，也可能是注释
        if (peek(tk) == '/') {
            // 偷看下下个字符
            char next_char = tk->src[tk->pos + 1]; // 简化写法，注意边界检查
            
            if (next_char == '/' || next_char == '*') {
                skip_comment(tk); // 是注释！交给 skip_comment 处理
                continue; // 处理完注释后，继续外层循环检查还有没有空格
            } else {
                break; // 只是个除号，不是空白，跳出循环去解析运算符
            }
        }
        
        // 普通空白，直接吃掉
        advance(tk);
    }
}
```

**难点：** `//` 注释要读到行尾，`/* ... */` 注释要一直读到 `*/`，甚至可能跨行。

---

## 3. 核心调度：`next()` 函数

这是 `src/tokenizer.c` 的灵魂。它根据当前字符的特征，将任务分发给不同的子函数。

```c
Token *next(Tokenizer *tk)
{
    // 1. 先打扫卫生
    skip_space(tk);

    // 2. 检查是否结束
    char ch = peek(tk);
    if (ch == '\0')
        return tokenize_eof(tk);

    // 3. 状态机分发 (Dispatcher)

    // 情况 A: 字母或下划线开头 -> 可能是关键字 (int)，也可能是变量名 (index)
    // 我们统一交给 tokenize_keyword 处理，让它去查表区分
    if (is_alpha(ch) || ch == '_')
        return tokenize_keyword(tk);

    // 情况 B: 数字开头 -> 解析数字 (123, 3.14)
    if (is_digit(ch))
        return tokenize_number(tk);

    // 情况 C: 特殊符号开头
    switch (ch)
    {
    case '\'': return tokenize_char(tk);   // 'a'
    case '\"': return tokenize_string(tk); // "hello"
    case '#':  return tokenize_preprocessor(tk); // #include
    
    // 情况 D: 剩下的全是运算符 (标点符号)
    // (, ), +, -, *, /, =, <, > ...
    // 我们全部丢给 tokenize_operator，让它去贪婪匹配
    default:
        return tokenize_operator(tk);
    }
}
```

---

## 4. 驱动引擎：`tokenize_all`

最后，我们需要一个对外接口，能够一次性把整个文件解析完，并存入我们在第 6 章写的 `Vector` 中。

```c
Vector *tokenize_all(const char *src)
{
    Tokenizer *tk = tokenizer_new(src);
    if (!tk) return NULL;

    Vector *tokens = vector_new(sizeof(Token)); // 存 Token 结构体

    for (;;) // 死循环，直到 EOF
    {
        Token *t = next(tk);
        
        // 把解析出的 Token 放入动态数组
        vector_push_back(tokens, t); // 存入指针
        
        if (t->type == T_EOF)
            break;
    }

    // 释放 Tokenizer 上下文 (Token 本身已经在 vector 里了，不要 free Token)
    tokenizer_free(tk);
    return tokens;
}
```

---

## 5. 更新构建系统

我们需要把新拆分出来的 `.c` 文件全部加入 `CMakeLists.txt`。

```cmake
set(SOURCES
    src/main.c
    src/utils.c
    src/vector.c
    src/token.c
    src/tokenizer.c        # 调度中心
    src/tokenize_space.c   # 新增
    src/tokenize_literal.c # 新增
    src/tokenize_operator.c # 新增
)
```

---

### 下一章预告

骨架已经搭好了，现在 `next()` 会把任务分发出去。但具体的解析逻辑（比如如何区分 `int` 和 `interface`，如何区分 `+` 和 `+=`）还没有细讲。

下一章 **[11. 复杂 Token 处理](11_tokenizer_details.md)**，我们将深入这三个子文件，攻克 **关键字查表**、**字符串转义** 和 **运算符贪婪匹配** 这三大难关。