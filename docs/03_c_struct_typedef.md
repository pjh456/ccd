# 03. 数据的容器：结构体、枚举与 Typedef

**本章目标**：

1. **痛点解决**：如何优雅地管理一组相关的数据（比如一个学生的姓名、年龄、学号）？
2. **核心语法**：彻底搞懂 `struct`（结构体）和 `enum`（枚举）。
3. **偷懒技巧**：使用 `typedef` 少写代码。
4. **实战应用**：为我们的项目定义核心数据结构 —— `Token`。

---

## 1. 为什么要用结构体 (Struct)？

想象一下，如果不使用结构体，我们要怎么在代码里表示“一个单词（Token）”的信息？
这个单词可能是 `int`，出现在第 5 行，第 12 列。

你可能会这样写：

```c
// 糟糕的代码
char *token1_text = "int";
int token1_line = 5;
int token1_col = 12;

char *token2_text = "main";
int token2_line = 5;
int token2_col = 16;
```

如果你有 1000 个单词怎么办？定义 3000 个变量吗？这简直是地狱。

**结构体 (Struct)** 就像是一个 **背包**。它允许你把不同类型的数据（名字、行号、列号）塞进同一个包里，作为一个整体来传递。

---

## 2. 给类型起别名 (Typedef)

在 C 语言中，定义结构体的原始语法有点啰嗦：

```c
struct Token {
    char *text;
    int line;
};

// 每次使用时，都必须带上 struct 关键字
struct Token t1; 
struct Token t2;
```

程序员都是很懒的。为了少写 `struct` 这 6 个字母，我们使用 **`typedef`**（Type Define，类型定义）。

它的作用就是 **给类型起个外号**。

### 现代写法（推荐）

我们将 `struct` 的定义和 `typedef` 合二为一：

```c
typedef struct {
    char *text;
    int line;
    int col;
} Token; 

// 现在，Token 就等同于 struct { ... }
// 我们可以直接这样写：
Token t1;
```

---

## 🚦 3. 给数字起名字 (Enum)

Token 有很多种类型：关键字 (`int`, `return`)、标识符 (变量名)、数字、符号 (`+`, `-`)。

如果我们用整数来表示类型：

- `0` 代表关键字
- `1` 代表数字
- `2` 代表符号...

代码里就会出现大量的 `if (type == 0)`。过两天你自己都忘了 `0` 是啥。

这就是可怕的 **魔术数字 (Magic Number)**。

**枚举 (Enum)** 专门用来解决这个问题：

```c
typedef enum {
    TOKEN_KEYWORD,    // 编译器自动赋值为 0
    TOKEN_NUMBER,     // 1
    TOKEN_IDENTIFIER, // 2
    TOKEN_EOF         // End Of File (文件结束标记)
} TokenType;

// 使用时：
TokenType type = TOKEN_KEYWORD; // 既清晰，又高效（底层还是 int）
```

---

## 4. 实战：定义 CCD 项目的核心数据结构

现在，我们要把这些知识用到刚才建立的工程里。

我们需要定义一个 `Token`，它是查重器的最小原子单位。

### 第一步：创建头文件

在 `include/` 目录下新建一个文件 `token.h`。

> **关于头文件保护符 (`#pragma once`)**
> 所有的 `.h` 文件第一行都应该写 `#pragma once`。
> 它的作用是：如果你的代码里不小心 `#include "token.h"` 了两次，编译器会聪明地只读取一次，防止“重定义错误”。
> 一般来说，主流编译器都支持这种语法，如果不支持请自行使用 **include guard**，此处不再赘述。

### 第二步：编写 `include/token.h`

请将以下代码写入文件：

```c
#pragma once

// 引入这个库是为了使用 size_t (表示大小/长度的标准类型)
#include <stddef.h> 

// 1. 定义 Token 的种类
typedef enum {
    T_UNKNOWN,      // 未知类型
    T_KEYWORD,      // 关键字 (int, return...)
    T_IDENTIFIER,   // 标识符 (main, a, b...)
    T_NUMBER,       // 数字 (123, 3.14)
    T_STRING,       // 字符串 ("hello")
    T_OPERATOR,     // 运算符 (+, -, ==)
    T_EOF           // 文件结束
} TokenType;

// 2. 定义 Token 结构体
typedef struct {
    TokenType type;     // 它是哪种类型？
    
    const char *start;  // 它是源码中的哪一段？(指向源码字符串的指针)
    size_t length;      // 它有多长？
    
    int line;           // 在第几行？
    int col;            // 在第几列？
} Token;

// 💡 为什么不用 char* text 存字符串副本？
// 答：为了性能！我们不需要拷贝 "int" 这三个字母，
// 只需要记录它在源代码中的【起始位置】和【长度】即可。
// 这种技巧叫做 "String View" (字符串视图)。
```

---

## 5. 在 `main.c` 中测试

虽然我们还没写词法分析器，但我们可以先手动捏一个 `Token` 试试，看看结构体好不好用。

修改 `src/main.c`：

```c
#include <stdio.h>
#include "token.h" // 引入我们刚才定义的结构体

// 辅助函数：打印 Token 信息
void print_token(Token t) {
    // %.*s 是 printf 的一个高级技巧
    // 它表示：打印字符串，但只打印 t.length 这么长
    printf("Line %d, Col %d: Type=%d, Content='%.*s'\n", 
           t.line, t.col, t.type, (int)t.length, t.start);
}

int main() {
    // 假设这是我们的源代码
    char source_code[] = "int main";

    // 手动构造一个 Token，代表 "int"
    Token t1;
    t1.type = T_KEYWORD;
    t1.start = source_code; // 指向字符串开头 'i'
    t1.length = 3;          // 长度为 3 ("int")
    t1.line = 1;
    t1.col = 1;

    // 手动构造一个 Token，代表 "main"
    Token t2;
    t2.type = T_IDENTIFIER;
    t2.start = source_code + 4; // 指针偏移 4 位，指向 'm'
    t2.length = 4;              // 长度为 4 ("main")
    t2.line = 1;
    t2.col = 5;

    print_token(t1);
    print_token(t2);

    return 0;
}
```

---

## 6. 编译运行

和之前一样，使用 CMake 进行构建：

```bash
cd build
cmake ..
cmake --build .
```

运行程序（Windows PowerShell）：
```bash
.\ccd_cli.exe
```

**预期输出：**
```text
Line 1, Col 1: Type=1, Content='int'
Line 1, Col 5: Type=2, Content='main'
```

如果你看到了这个输出，说明你已经成功掌握了结构体的定义、实例化和成员访问！

---

## 核心知识点回顾

1. **Struct**: 数据的背包，把零散的变量打包。
2. **Typedef**: 给类型取别名，少写代码。
3. **Enum**: 消除魔术数字，让代码可读性更高。
4. **指针偏移**: `source_code + 4` 这种写法是不是很神奇？它直接跳过了前 4 个字符。这是 C 语言指针操作的魅力，下一章我们将深入探讨内存与指针。

---

### 下一章预告

我们在 `main` 函数里手动创建了 `Token`，但这太麻烦了。
如果我们读入了一个很大的文件，我们需要动态地创建成千上万个 `Token`。
这就涉及到 C 语言中最难也是最核心的部分 —— **内存管理 (malloc/free)**。

下一章 **[04. 指针与堆内存](04_c_memory_pointers.md)**，我们将直面这个“大魔王”，彻底理解栈和堆的区别。