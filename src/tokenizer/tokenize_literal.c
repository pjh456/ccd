#include "tokenizer.h"
#include "tokenizer_impl/tokenizer_impl.h"
#include "tokenizer_impl/token.h"
#include "utils.h"

#include <string.h>

// 关键字映射表
typedef struct
{
    const char *kw;
    TokenType type;
} KwEntry;

// 关键字查找表
// 使用简单的线性查找（可用哈希表或 Trie 树优化）
static const KwEntry kw_table[] = {

    {"extern", T_EXTERN},
    {"static", T_STATIC},
    {"inline", T_INLINE},
    {"register", T_REGISTER},
    {"restrict", T_RESTRICT},
    {"volatile", T_VOLATILE},
    {"const", T_CONST},

    {"auto", T_AUTO},
    {"void", T_VOID},
    {"signed", T_SIGNED},
    {"unsigned", T_UNSIGNED},
    {"char", T_CHAR},
    {"short", T_SHORT},
    {"int", T_INT},
    {"long", T_LONG},
    {"float", T_FLOAT},
    {"double", T_DOUBLE},

    {"do", T_DO},
    {"while", T_WHILE},
    {"for", T_FOR},
    {"continue", T_CONTINUE},
    {"break", T_BREAK},
    {"if", T_IF},
    {"else", T_ELSE},
    {"switch", T_SWITCH},
    {"case", T_CASE},
    {"default", T_DEFAULT},
    {"return", T_RETURN},
    {"goto", T_GOTO},

    {"enum", T_ENUM},
    {"sizeof", T_SIZEOF},
    {"struct", T_STRUCT},
    {"typedef", T_TYPEDEF},
    {"union", T_UNION},

    {NULL, T_UNKNOWN},
};

Token *tokenize_keyword(Tokenizer *tk)
{
    Token *t = make_token(tk, T_UNKNOWN, 0, 1);
    const char *const p = tk->src + tk->pos;

    // 尝试匹配关键字
    for (const KwEntry *e = kw_table; e->kw; e++)
    {
        size_t len = strlen(e->kw);

        // 1. 检查长度是否越界
        // 2. 检查字符串是否匹配
        if (tk->pos + len <= tk->len &&
            strncmp(p, e->kw, len) == 0)
        {
            // 关键点：检查关键字边界
            // 例如源代码是 "interface"，匹配 "int" 后，下一个字符是 'e' (isalnum)，
            // 这说明它不是 "int" 关键字，而是标识符的一部分。
            if (!is_alnum(p[len]))
            {
                t->type = e->type;
                t->str = str_n_clone(p, len);
                t->length = len;

                // 更新位置
                tk->pos += len;
                tk->stus.col += len;
                return t;
            }
        }
    }

    // 如果不是关键字，说明是普通标识符 (Identifier)
    // 规则：消耗所有字母、数字和下划线
    while (is_alnum(peek(tk)))
        advance(tk);

    t->type = T_IDENTIFIER;
    t->length = tk->src + tk->pos - p; // 计算长度
    t->str = str_n_clone(p, t->length);

    return t;
}

Token *tokenize_number(Tokenizer *tk)
{
    Token *t = make_token(tk, T_NUMBER, NULL, 0);
    const char *const p = tk->src + tk->pos;
    // 1. 消耗整数部分
    while (is_digit(peek(tk)))
        advance(tk);

    // 2. 处理小数部分
    if (peek(tk) == '.')
    {
        advance(tk); // 消耗 '.'
        while (is_digit(peek(tk)))
            advance(tk);
    }

    t->length = tk->src + tk->pos - p;
    t->str = str_n_clone(p, t->length);

    return t;
}

Token *tokenize_char(Tokenizer *tk)
{
    Token *t = make_token(tk, T_CHARACTER, NULL, 0);
    const char *const p = tk->src + tk->pos;
    advance(tk); // 消耗开头的 '

    while (peek(tk) != '\'')
    {
        char ch = peek(tk);

        // 非法：字符字面量没闭合
        if (ch == '\0' || ch == '\n')
            break;

        if (ch == '\\')
        {
            advance(tk); // 消耗 '\'
            if (peek(tk) != '\0')
                advance(tk); // 消耗转义字符，比如  'n', '\'', '\\'
            continue;
        }

        advance(tk);
    }
    advance(tk); // 消耗闭合的 '

    t->length = tk->src + tk->pos - p;
    t->str = str_n_clone(p, t->length);

    return t;
}

Token *tokenize_string(Tokenizer *tk)
{
    Token *t = make_token(tk, T_STRING, NULL, 0);
    const char *const p = tk->src + tk->pos;

    advance(tk); // 消耗开头的 "

    while (peek(tk) != '\"')
    {
        char ch = peek(tk);

        // 非法：字符串字面量没闭合
        if (ch == '\0' || ch == '\n')
            break;

        if (ch == '\\')
        {
            advance(tk); // 消耗 '\'
            if (peek(tk) != '\0')
                advance(tk); // 消耗转义字符，比如  'n', '\'', '\\'
            continue;
        }

        advance(tk);
    }
    advance(tk); // 消耗闭合的 "

    t->length = tk->src + tk->pos - p;
    t->str = str_n_clone(p, t->length);

    return t;
}