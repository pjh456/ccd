#include "tokenizer.h"
#include "tokenizer_impl/tokenizer_impl.h"
#include <string.h>

// 运算符映射表
typedef struct
{
    const char *op; // 符号文本，如 ">>="
    TokenType type; // 对应的枚举
} OpEntry;

// 运算符表
// 注意：顺序在逻辑上不严格要求长字符在前，因为代码中实现了遍历寻找最长匹配的算法。
static const OpEntry op_table[] = {

    // ===== 长度 3 =====
    {"<<=", T_LEFT_SHIFT_ASSIGN},
    {">>=", T_RIGHT_SHIFT_ASSIGN},

    // ===== 长度 2 =====
    {"->", T_ARROW},
    {"==", T_EQUAL},
    {"!=", T_NOT_EQUAL},
    {"<=", T_LESS_EQUAL},
    {">=", T_GREATER_EQUAL},

    {"&&", T_AND_AND},
    {"||", T_OR_OR},

    {"+=", T_PLUS_ASSIGN},
    {"-=", T_MINUS_ASSIGN},
    {"*=", T_MUL_ASSIGN},
    {"/=", T_DIV_ASSIGN},
    {"%=", T_MOD_ASSIGN},

    {"&=", T_AND_ASSIGN},
    {"|=", T_OR_ASSIGN},
    {"^=", T_XOR_ASSIGN},

    {"<<", T_LEFT_SHIFT},
    {">>", T_RIGHT_SHIFT},

    {"++", T_INC},
    {"--", T_DEC},

    // ===== 单字符运算符 =====
    {"!", T_NOT},
    {"~", T_TILDE},
    {"&", T_AND},
    {"|", T_OR},
    {"^", T_XOR},

    {"+", T_PLUS},
    {"-", T_MINUS},
    {"*", T_STAR},
    {"/", T_DIV},
    {"%", T_MOD},
    {"=", T_ASSIGN},

    {"<", T_LESS},
    {">", T_GREATER},

    {"#", T_PREPROCESS},
    {"\\", T_BACKSLASH},

    {"(", T_LEFT_PAREN},
    {")", T_RIGHT_PAREN},
    {"[", T_LEFT_BRACKET},
    {"]", T_RIGHT_BRACKET},
    {"{", T_LEFT_BRACE},
    {"}", T_RIGHT_BRACE},

    {",", T_COMMA},
    {":", T_COLON},
    {";", T_SEMICOLON},
    {".", T_DOT},
    {"?", T_QUESTION},

    {NULL, T_UNKNOWN},
};

Token tokenize_operator(Tokenizer *tk)
{
    Token t = make_token(tk, T_UNKNOWN, 1); // 预设一个未知 Token
    const char *p = t.start;                // 获取源码当前位置指针

    const OpEntry *best = NULL;
    size_t best_len = 0;

    // === 贪婪匹配算法 (Maximal Munch) ===
    // 假设源码是 ">>= a"，如果不贪婪，可能匹配成 ">" (大于号)
    // 我们必须找到能匹配上的、最长的那个符号。
    for (const OpEntry *e = op_table; e->op; e++)
    {
        size_t len = strlen(e->op);

        // 1. 检查长度是否越界
        // 2. 检查字符串是否匹配
        if (tk->pos + len <= tk->len &&
            strncmp(p, e->op, len) == 0)
        {
            // 如果匹配成功，且比当前记录的 best 还长，就更新
            if (len > best_len)
            {
                best = e;
                best_len = len;
            }
        }
    }

    if (best)
    {
        // 找到了最长匹配
        t.type = best->type;
        t.length = best_len;

        // 更新 Tokenizer 状态
        tk->pos += best_len;
        tk->stus.col += best_len;
    }
    else // 这里的逻辑主要是处理边界，正常 switch 进来的应该都能匹配
        advance(tk);
    return t;
}