#define ENABLE_DEBUG

#include "tokenizer.h"
#include "tokenizer_impl/tokenizer_impl.h"
#include "utils.h"
#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 构造函数：初始化 Tokenizer
Tokenizer *tokenizer_new(const char *src)
{
    Tokenizer *tk = malloc(sizeof(*tk));
    tk->src = src, tk->pos = 0;
    tk->len = strlen(src);            // 注意：这里需要 O(N) 时间扫描长度
    tk->stus.line = tk->stus.col = 1; // 行、列号从 1 开始
    return tk;
}

// 析构函数：释放 Tokenizer 内存
void tokenizer_free(Tokenizer *tk)
{
    if (!tk)
        return;
    free(tk); // 只释放结构体本身，src 是外部传入的，不归我们需要释放
}

// 偷看一眼：返回当前字符，但不移动光标
char peek(Tokenizer *tk)
{
    return (tk->pos < tk->len) ? tk->src[tk->pos] : '\0';
}

// 步进：返回当前字符，并将光标向前移动一格
// 同时负责更新行号和列号
char advance(Tokenizer *tk)
{
    // 如果读到了换行符，行号+1，列号重置
    if (peek(tk) == '\n')
        tk->stus.line++, tk->stus.col = 1;
    else if (peek(tk) == '\r')
    {
        tk->pos++;
        return advance(tk);
    }
    else
        tk->stus.col++;

    if (tk->pos < tk->len)
        return tk->src[tk->pos++]; // 返回当前字符并 pos++
    return '\0';
}

// === 核心调度逻辑 ===
// 这是词法分析器的主循环入口
Token *next(Tokenizer *tk)
{
    // 1. 跳过无意义的字符（空格、换行、制表符、注释等）
    skip_space(tk);

    // 2. 检查是否结束
    char ch = peek(tk);
    if (ch == '\0')
        return tokenize_eof(tk);

    // 3. 状态机分发 (Dispatcher)
    // 根据首字符的特征，决定调用哪个子函数

    // 情况 A: 是字母或下划线 -> 可能是关键字，也可能是标识符
    if (is_alpha(ch))
        return tokenize_keyword(tk);

    // 情况 B: 是数字 -> 解析数字字面量
    if (is_digit(ch))
        return tokenize_number(tk);

    // 情况 C: 符号处理
    switch (ch)
    {
    case '\'': // 单引号 -> 字符
        return tokenize_char(tk);
    case '\"': // 双引号 -> 字符串
        return tokenize_string(tk);
    case '#': // 井号 -> 预处理
        return tokenize_preprocessor(tk);
    // 其余所有标点符号，统一交给 operator 处理
    // 那里会有更复杂的贪婪匹配逻辑 (比如区分 + 和 ++)
    case '<':
    case '>':
    case '=':
    case '(':
    case ')':
    case '[':
    case ']':
    case '{':
    case '}':
    case '!':
    case '&':
    case '|':
    case '^':
    case '~':
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case '\\':
    case '?':
    case ',':
    case '.':
    case ':':
    case ';':
        return tokenize_operator(tk);
    default:
        return tokenize_unknown(tk);
    }
}

Vector *tokenize_all(const char *src)
{
    Tokenizer *tk = tokenizer_new(src);
    if (!tk)
        return NULL;

    Vector *tokens = vector_new(sizeof(Token));
    if (!tokens)
        return NULL;

    for (;;)
    {
        Token *t = next(tk);
        vector_push_back(tokens, t);
        if (t->type == T_EOF)
            break;
    }

    tokenizer_free(tk);
    return tokens;
}