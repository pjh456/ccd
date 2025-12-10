# 09. 给单词办理身份证：完善 Token 定义

**本章目标**：

1.  **枚举爆炸**：将 `token.h` 中的 `TokenType` 扩充到能覆盖 C 语言常用语法的程度。
2.  **调试辅助**：编写一个 helper 函数，能把 `T_INT` 这种整数转换回字符串 "INT"，方便我们打印调试。
3.  **代码组织**：创建 `src/token.c` 来存放 Token 相关的实现代码。

---

## 1. 扩充 TokenType

之前在第 3 章，我们只定义了简单的 `T_KEYWORD` 和 `T_OPERATOR`。

这对于真正的编译器来说太粗糙了。

Parser（语法分析器）需要精确地知道：这个 `T_OPERATOR` 到底是加号 `+` 还是乘号 `*`？这个 `T_KEYWORD` 到底是 `if` 还是 `while`？

我们需要把它们全部拆开。

对 `include/token.h` 进行彻底重构：

```c
#pragma once

#include <stddef.h>

typedef enum {
    // === 基础类型 ===
    T_IDENTIFIER, // 标识符 (变量名、函数名，如 main, count)
    T_NUMBER,     // 数字字面量 (123, 3.14)
    T_CHARACTER,  // 字符字面量 ('a')
    T_STRING,     // 字符串字面量 ("hello")

    // === 位运算符 ===
    T_NOT,         // ! (逻辑非)
    T_TILDE,       // ~ (按位取反)
    T_AND,         // &
    T_OR,          // |
    T_XOR,         // ^
    T_LEFT_SHIFT,  // <<
    T_RIGHT_SHIFT, // >>

    // === 算术运算符 ===
    T_PLUS,   // +
    T_MINUS,  // -
    T_STAR,   // *
    T_DIV,    // /
    T_MOD,    // %
    T_ASSIGN, // =

    // === 比较/逻辑运算符 ===
    T_LESS,          // <
    T_GREATER,       // >
    T_EQUAL,         // ==
    T_NOT_EQUAL,     // !=
    T_LESS_EQUAL,    // <=
    T_GREATER_EQUAL, // >=
    T_AND_AND,       // &&
    T_OR_OR,         // ||

    // === 复合赋值位运算符 (&=, <<= 等) ===
    T_AND_ASSIGN,         // &=
    T_OR_ASSIGN,          // |=
    T_XOR_ASSIGN,         // ^=
    T_LEFT_SHIFT_ASSIGN,  // <<=
    T_RIGHT_SHIFT_ASSIGN, // <<=

    // === 复合赋值算数运算符 (+=, -= 等) ===
    T_PLUS_ASSIGN,  // +=
    T_MINUS_ASSIGN, // -=
    T_MUL_ASSIGN,   // *=
    T_DIV_ASSIGN,   // /=
    T_MOD_ASSIGN,   // %=

    // === 自增自减 ===
    T_INC, // ++
    T_DEC, // --

    // === 特殊符号 ===
    T_PREPROCESS, // # (预处理指令)
    T_BACKSLASH,  // \ (反斜杠)

    // === 分隔符 ===
    T_LEFT_PAREN,    // (
    T_RIGHT_PAREN,   // )
    T_LEFT_BRACKET,  // [
    T_RIGHT_BRACKET, // ]
    T_LEFT_BRACE,    // {
    T_RIGHT_BRACE,   // }

    T_COMMA,     // ,
    T_COLON,     // :
    T_SEMICOLON, // ;
    T_DOT,       // .
    T_ARROW,     // ->
    T_QUESTION,  // ?

    // === C 语言关键字 (C11 标准) ===
    T_EXTERN,   // extern
    T_STATIC,   // static
    T_INLINE,   // inline
    T_REGISTER, // register
    T_RESTRICT, // restrict
    T_VOLATILE, // volalite
    T_CONST,    // const

    T_AUTO,     // auto
    T_VOID,     // void
    T_SIGNED,   // signed
    T_UNSIGNED, // unsigned
    T_CHAR,     // char
    T_SHORT,    // short
    T_INT,      // int
    T_LONG,     // long
    T_FLOAT,    // float
    T_DOUBLE,   // double

    T_DO,       // do
    T_WHILE,    // while
    T_FOR,      // for
    T_CONTINUE, // continue
    T_BREAK,    // break
    T_IF,       // if
    T_ELSE,     // else
    T_SWITCH,   // switch
    T_CASE,     // case
    T_DEFAULT,  // default
    T_RETURN,   // return
    T_GOTO,     // goto

    T_ENUM,    // enum
    T_SIZEOF,  // sizeof
    T_STRUCT,  // struct
    T_TYPEDEF, // typedef
    T_UNION,   // union

    // === 状态控制 ===
    T_EOF,    // End Of File (文件结束)
    T_UNKNOWN // 无法识别的字符
} TokenType;

typedef struct {
    TokenType type;
    const char *start;  // 指向源码的指针
    size_t length;      // Token 长度
    int line;           // 行号
    int col;            // 列号
} Token;

// 新增：打印 Token 类型的名字 (调试用)
// 输入: T_INT -> 返回: "INT"
const char* token_type_name(TokenType type);

// 新增：打印 Token 的详细信息
void print_token(const Token *t);
```

---

## 2. 让 Token 会“说话”

在调试时，如果我们打印 `token.type`，只能看到一个整数（比如 `4` 或 `12`）。天知道 `12` 代表什么。

我们需要一个函数，把 `enum` 映射回字符串。

因此我们需要在 `src/token.c` 里进行实现：
```c
const char *token_name(TokenType tt)
{
    switch (tt)
    {
    case T_IDENTIFIER:
        return "IDENTIFIER";
    case T_NUMBER:
        return "NUMBER";
    case T_CHARACTER:
        return "CHARACTER";
    case T_STRING:
        return "STRING";

    case T_NOT:
        return "NOT";
    case T_TILDE:
        return "TILDE";
    case T_AND:
        return "AND";
    case T_OR:
        return "OR";
    case T_XOR:
        return "XOR";
    case T_LEFT_SHIFT:
        return "LEFT_SHIFT";
    case T_RIGHT_SHIFT:
        return "RIGHT_SHIFT";

    case T_PLUS:
        return "PLUS";
    case T_MINUS:
        return "MINUS";
    case T_STAR:
        return "STAR";
    case T_DIV:
        return "DIV";
    case T_MOD:
        return "MOD";
    case T_ASSIGN:
        return "ASSIGN";

    case T_LESS:
        return "LESS";
    case T_GREATER:
        return "GREATER";
    case T_EQUAL:
        return "EQUAL";
    case T_NOT_EQUAL:
        return "NOT_EQUAL";
    case T_LESS_EQUAL:
        return "LESS_EQUAL";
    case T_GREATER_EQUAL:
        return "GREATER_EQUAL";
    case T_AND_AND:
        return "AND_AND";
    case T_OR_OR:
        return "OR_OR";

    case T_AND_ASSIGN:
        return "AND_ASSIGN";
    case T_OR_ASSIGN:
        return "OR_ASSIGN";
    case T_XOR_ASSIGN:
        return "XOR_ASSIGN";
    case T_LEFT_SHIFT_ASSIGN:
        return "LEFT_SHIFT_ASSIGN";
    case T_RIGHT_SHIFT_ASSIGN:
        return "RIGHT_SHIFT_ASSIGN";

    case T_PLUS_ASSIGN:
        return "PLUS_ASSIGN";
    case T_MINUS_ASSIGN:
        return "MINUS_ASSIGN";
    case T_MUL_ASSIGN:
        return "MUL_ASSIGN";
    case T_DIV_ASSIGN:
        return "DIV_ASSIGN";
    case T_MOD_ASSIGN:
        return "MOD_ASSIGN";
    case T_INC:
        return "INC";
    case T_DEC:
        return "DEC";

    case T_PREPROCESS:
        return "PREPROCESS";
    case T_BACKSLASH:
        return "BACKSLASH";

    case T_LEFT_PAREN:
        return "LEFT_PAREN";
    case T_RIGHT_PAREN:
        return "RIGHT_PAREN";
    case T_LEFT_BRACKET:
        return "LEFT_BRACKET";
    case T_RIGHT_BRACKET:
        return "RIGHT_BRACKET";
    case T_LEFT_BRACE:
        return "LEFT_BRACE";
    case T_RIGHT_BRACE:
        return "RIGHT_BRACE";

    case T_COMMA:
        return "COMMA";
    case T_COLON:
        return "COLON";
    case T_SEMICOLON:
        return "SEMICOLON";
    case T_DOT:
        return "DOT";
    case T_ARROW:
        return "ARROW";
    case T_QUESTION:
        return "QUESTION";

    case T_EXTERN:
        return "EXTERN";
    case T_STATIC:
        return "STATIC";
    case T_INLINE:
        return "INLINE";
    case T_REGISTER:
        return "REGISTER";
    case T_RESTRICT:
        return "RESTRICT";
    case T_VOLATILE:
        return "VOLATILE";
    case T_CONST:
        return "CONST";

    case T_AUTO:
        return "AUTO";
    case T_VOID:
        return "VOID";
    case T_SIGNED:
        return "SIGNED";
    case T_UNSIGNED:
        return "UNSIGNED";
    case T_CHAR:
        return "CHAR";
    case T_SHORT:
        return "SHORT";
    case T_INT:
        return "INT";
    case T_LONG:
        return "LONG";
    case T_FLOAT:
        return "FLOAT";
    case T_DOUBLE:
        return "DOUBLE";

    case T_DO:
        return "DO";
    case T_WHILE:
        return "WHILE";
    case T_FOR:
        return "FOR";
    case T_CONTINUE:
        return "CONTINUE";
    case T_BREAK:
        return "BREAK";
    case T_IF:
        return "IF";
    case T_ELSE:
        return "ELSE";
    case T_SWITCH:
        return "SWITCH";
    case T_CASE:
        return "CASE";
    case T_DEFAULT:
        return "DEFAULT";
    case T_RETURN:
        return "RETURN";
    case T_GOTO:
        return "GOTO";

    case T_ENUM:
        return "ENUM";
    case T_SIZEOF:
        return "SIZEOF";
    case T_STRUCT:
        return "STRUCT";
    case T_TYPEDEF:
        return "TYPEDEF";
    case T_UNION:
        return "UNION";

    case T_EOF:
        return "EOF";
    case T_UNKNOWN:
    default:
        return "UNKNOWN";
    }
}

void print_token(const Token *t)
{
    // 格式化输出：
    // 行:列  [类型名称]  "原始内容"
    printf(
        "%s (%.*s) at %d:%d\n",
        token_name(t->type),
        (int)t->length, t->start,
        t->line, t->col);
}
```

---

## 3. 更新构建系统

既然新增了 `src/token.c`，别忘了去 `CMakeLists.txt` 报到。

```cmake
set(SOURCES
    src/main.c
    src/vector.c
    src/utils.c
    src/token.c   # <--- 新增
)
```

---

## 4. 验证测试 (选做)

如果你想验证这些改动，可以简单修改 `src/main.c`，把我们在第 6 章测试用的代码改一下，使用新的枚举值：
```c
// 原来是 T_KEYWORD, 现在改成 T_INT
Token t1 = {T_INT, "int", 3, 1, 1};

// 使用新函数打印
print_token(&t1);
```

运行结果应该类似于：
```text
1:1     INT          "int"
```

---

## 设计哲学：为什么分这么细？

你可能会问：“为什么 `+` 和 `-` 不合并成一个 `T_OPERATOR`，然后在属性里记下来它是加还是减？”

这是一个很好的问题。

在简单的解释器中可以这样做。但在构建 **AST (抽象语法树)** 时，让每个运算符有独立的 `TokenType` 会让代码更清晰。

例如，在解析表达式时，我们可以直接判断：
```c
if (token.type == T_PLUS) { ... }
else if (token.type == T_STAR) { ... }
```

这比写 `if (token.type == T_OP && token.val == OP_PLUS)` 要直观得多。

---

### 下一章预告

身份证办理完毕！现在我们有了数据结构 (`Vector`)，有了数据定义 (`Token` 枚举)，也有了文件内容 (`char*`)。

万事俱备，只欠东风。

下一章 **[10. 词法分析器实现：扫描循环](10_tokenizer_loop.md)**，我们将编写 Tokenizer 的心脏 —— **`next_token()`** 函数。我们将亲手实现那个“吞吐字符”的死循环，让计算机真正开始阅读代码。