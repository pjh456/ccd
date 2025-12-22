#pragma once

#include <stdint.h>

// 前向声明，告诉编译器 Tokenizer 是个类型，具体细节在别处
typedef struct Token Token;
typedef struct Tokenizer Tokenizer;

/**
 * @brief Token 类型枚举
 * 使用枚举是为了避免代码中出现 "魔术数字"，让 switch-case 结构更清晰
 */
typedef enum
{
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
    T_ELLIPSIS,  // ...

    // === C 语言关键字 (C11 标准) ===
    T_EXTERN,   // extern
    T_STATIC,   // static
    T_INLINE,   // inline
    T_REGISTER, // register
    T_RESTRICT, // restrict
    T_VOLATILE, // volatile
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

/**
 * @brief Token 结构体
 * 采用 String View (视图) 模式，不拷贝字符串，只记录指针和长度，
 * 极大提高了 Tokenizer 的性能。
 */
struct Token
{
    TokenType type; // 类别
    char *str;      // Token 原始字符串
    int line;       // 所在的行号
    int col;        // 所在的列号
};

// 获取 Token 类型的字符串名称 (用于调试打印)
const char *token_name(TokenType tt);

// 打印 Token 详细信息
void print_token(const Token *t);

// 构造一个 Token 的辅助函数
Token *make_token(Tokenizer *tk, TokenType tt, const char *lit, size_t len);

void token_free(Token *t);