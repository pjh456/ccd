#include "tokenizer_impl/token.h"
#include "tokenizer.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>

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
        return "PREPROCESSOR";
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
    case T_ELLIPSIS:
        return "ELLIPSIS";

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
        (int)t->length, t->str,
        t->line, t->col);
}

Token *make_token(Tokenizer *tk, TokenType tt, const char *lit, size_t len)
{
    Token *t = malloc(sizeof(*t));
    t->type = tt;
    t->str = str_n_clone(lit, len);
    t->length = len;
    if (tk)
    {
        // 记录生成该 Token 时的行号和列号
        t->line = tk->stus.line;
        t->col = tk->stus.col;
    }
    return t;
}

void token_free(Token *t)
{
    if (!t)
        return;
    if (t->str)
        free(t->str);
    free(t);
}