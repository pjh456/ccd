# 03. 数据的容器：结构体、枚举与 Typedef

**本章目标**：

1.  **痛点解决**：如何优雅地管理一组相关的数据（比如一个 Token 的类型、内容、行号）？
2.  **核心语法**：彻底搞懂 `struct`（结构体）和 `enum`（枚举）。
3.  **工程决策**：**String View (视图)** vs **Deep Copy (深拷贝)** —— 为什么我们在本项目中选择牺牲一点性能来换取安全性？
4.  **实战应用**：定义核心数据结构 —— `Token`。

---

## 1. 为什么要用结构体 (Struct)？

想象一下，如果不使用结构体，我们要怎么在代码里表示“一个单词（Token）”的信息？
这个单词可能是 `int`，出现在第 5 行，第 12 列。

你可能会这样写：

```c
// 糟糕的代码：变量满天飞
char *token1_text = "int";
int token1_line = 5;
int token1_col = 12;

char *token2_text = "main";
int token2_line = 5;
int token2_col = 16;
```

如果你有 1000 个单词怎么办？定义 3000 个变量吗？这简直是地狱。

**结构体 (Struct)** 就像是一个 **背包**。它允许你把不同类型的数据（字符串、整数）塞进同一个包里，作为一个整体来传递。

---

## 2. 给类型起别名 (Typedef)

在 C 语言中，定义结构体的原始语法有点啰嗦：

```c
struct Token {
    char *str;
    int line;
};

// 每次使用时，都必须带上 struct 关键字
struct Token t1; 
```

程序员都是很懒的。为了少写 `struct` 这 6 个字母，我们使用 **`typedef`**（Type Define，类型定义）。它的作用就是 **给类型起个外号**。

我们将 `struct` 的定义和 `typedef` 合二为一：

```c
typedef struct {
    char *str;
    int line;
    int col;
} Token; 

// 现在，Token 就等同于 struct { ... }
// 我们可以直接这样写：
Token t1;
```

---

## 3. 给数字起名字 (Enum)

Token 有很多种类型：关键字 (`int`)、标识符 (变量名)、数字、符号 (`+`)。
如果我们用整数来表示类型：`0` 代表关键字，`1` 代表数字... 代码里就会出现 `if (type == 0)`。
这就是可怕的 **魔术数字 (Magic Number)**。过两天你自己都忘了 `0` 是啥。

**枚举 (Enum)** 专门用来解决这个问题：

```c
typedef enum {
    T_KEYWORD,    // 编译器自动赋值为 0
    T_NUMBER,     // 1
    T_IDENTIFIER, // 2
    T_EOF         // End Of File
} TokenType;

// 使用时：
TokenType type = T_KEYWORD; // 既清晰，又高效（底层还是 int）
```

---

## 4. 关键设计决策：视图 vs 拥有 (View vs Own)

在定义 `Token` 结构体时，我们面临一个重大的架构选择：**如何存储 Token 的字符串内容？**

### 方案 A：String View (字符串视图)
只存储指向源代码字符串的指针和长度。

```c
typedef struct {
    const char *start; // 指向 source_code 的某个位置
    size_t len;        // 长度
} Token;
```

*   **优点**：**极快**。不需要 `malloc`，不需要 `strcpy`，零拷贝。
*   **缺点**：**生命周期耦合**。Token 的生存期必须短于源代码字符串。如果你释放了源代码 (`free(source_code)`)，所有的 Token 瞬间变成野指针。在复杂的解析过程中，这非常容易导致 Bug。

### 方案 B：Deep Copy / Ownership (深拷贝/独立拥有)
每个 Token 都有自己独立的字符串副本。

```c
typedef struct {
    char *str; // malloc 出来的新内存，存储 "int"
} Token;
```

*   **优点**：**安全、解耦**。Token 是独立的，哪怕源代码被释放了，Token 依然完好无损。这对于构建 AST（抽象语法树）非常有利，因为 AST 可能会活得比词法分析阶段更久。
*   **缺点**：**慢**。每次生成 Token 都要申请内存、拷贝字符串。

### 我们的选择：方案 B (深拷贝)

**理由**：我们是在写教育性质的项目，**安全性 (Safety) > 极致性能 (Performance)**。
采用深拷贝可以让 `Token` 的生命周期独立管理，避免初学者在后续章节因为内存释放顺序问题而陷入调试地狱。虽然有性能损耗，但在查重这个场景下完全可以接受。

---

## 5. 实战：编写 `include/token.h`

现在，在 `include/` 目录下新建文件 `token.h`。
我们将定义完整的 `TokenType` 枚举（包含 C 语言所有关键字）和 `Token` 结构体。

```c
#pragma once

#include <stddef.h> // size_t
#include <stdint.h> 

// 前向声明 (Forward Declaration)：
// 告诉编译器 "有 Tokenizer 这么个东西"，具体细节以后再说。
// 这样可以避免头文件互相引用的死循环。
typedef struct Tokenizer Tokenizer;

/**
 * @brief Token 类型枚举
 * 涵盖了 C 语言的关键字、运算符、分隔符等
 */
typedef enum {
    // === 基础类型 ===
    T_IDENTIFIER, // 变量名 main, count
    T_NUMBER,     // 123
    T_STRING,     // "hello"
    T_CHAR,       // 'a' (注意：这里也是关键字 char 的枚举，需根据上下文区分，或拆分定义)
    
    // === 运算符 ===
    T_PLUS, T_MINUS, T_STAR, T_DIV, T_MOD, // + - * / %
    T_ASSIGN, // =
    
    // === 比较符 ===
    T_EQUAL, T_NOT_EQUAL, // == !=
    T_GREATER, T_LESS,    // > <

    // === 括号 ===
    T_LEFT_PAREN, T_RIGHT_PAREN,   // ( )
    T_LEFT_BRACE, T_RIGHT_BRACE,   // { }
    T_SEMICOLON,                   // ;

    // === 关键字 (部分示例) ===
    T_INT, T_VOID, T_RETURN, T_IF, T_ELSE, T_WHILE, T_FOR,

    // === 控制 ===
    T_EOF,    // 文件结束
    T_UNKNOWN // 未知字符
} TokenType;

/**
 * @brief Token 结构体 (Deep Copy 模式)
 * 每个 Token 拥有自己独立的字符串内存。
 */
typedef struct {
    TokenType type;     // 类别
    char *str;          // 【独立拥有】Token 的内容副本 (需 malloc)
    int line;           // 行号
    int col;            // 列号
} Token;

// --- 函数声明 (接口) ---

// 构造一个 Token (会进行字符串拷贝)
Token *token_new(TokenType type, const char *str_start, size_t len, int line, int col);

// 释放 Token (显式释放 str 内存)
void token_free(Token *t);

// 打印 Token 详细信息 (用于调试)
void print_token(const Token *t);
```

---

## 6. 在 `main.c` 中测试

虽然我们还没实现 `token_new`（这涉及到内存分配，下一章讲），但我们可以手动在栈上模拟一个 `Token` 来验证结构体是否好用。

修改 `src/main.c`：

```c
#include <stdio.h>
#include "token.h"

// 暂时实现一个简单的打印函数
void print_token(const Token *t) {
    printf("Line %d, Col %d: Type=%d, Content='%s'\n", 
           t->line, t->col, t->type, t->str);
}

int main() {
    printf("Testing Token Struct...\n");

    // 手动构造一个 Token
    // 注意：这里我们将字符串字面量赋值给 str
    // 在正式代码中，这里应该是 malloc 出来的堆内存
    Token t1;
    t1.type = T_INT;
    t1.str = "int"; // 暂时指向静态区字符串，正式版这里要用深拷贝
    t1.line = 1;
    t1.col = 1;

    Token t2;
    t2.type = T_IDENTIFIER;
    t2.str = "main";
    t2.line = 1;
    t2.col = 5;

    print_token(&t1);
    print_token(&t2);

    return 0;
}
```

---

## 7. 编译运行

```bash
cd build
cmake ..
cmake --build .
```

运行结果：
```text
Testing Token Struct...
Line 1, Col 1: Type=25, Content='int'   <-- Type 具体的数字取决于枚举顺序
Line 1, Col 5: Type=0, Content='main'
```

---

## 核心知识点回顾

1.  **Struct**: 数据的背包。
2.  **Typedef**: 给类型取别名，少写 `struct`。
3.  **Enum**: 消除魔术数字。
4.  **架构权衡**: 我们选择了 **Deep Copy** 模式。
    *   这意味着：我们在创建 Token 时必须 `malloc` 内存。
    *   这也意味着：我们在使用完 Token 后必须 `free` 内存。

在 C 语言中，**“自己申请的内存，必须自己释放”** 是一条铁律。

### 下一章预告

我们在 `main.c` 里偷懒了，直接让 `t1.str = "int"`。但在实际读取文件时，我们需要根据文件内容动态截取字符串，并申请新的内存空间。

下一章 **[04. 指针与堆内存](04_c_memory_pointers.md)**，我们将直面 C 语言的“大魔王” —— `malloc`、`free` 以及如何避免内存泄漏。做好准备，这将是 C 语言最陡峭的学习曲线。