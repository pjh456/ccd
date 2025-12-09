#include <token.h>
#include <tokenizer.h>
#include <stdio.h>

const char *token_name(TokenType tt)
{
    switch (tt)
    {
    case T_IDENTIFIER:
        return "IDENTIFIER";
    case T_NUMBER:
        return "NUMBER";
    case T_CHAR:
        return "CHAR";
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

    case T_EOF:
        return "EOF";
    case T_UNKNOWN:
    default:
        return "UNKNOWN";
    }
}

void print_token(const Token *t)
{
    printf(
        "%s (%.*s) at %d:%d\n",
        token_name(t->type),
        (int)t->length, t->start,
        t->line, t->col);
}

Token make_token(Tokenizer *tk, TokenType tt, size_t len)
{
    Token t;
    t.type = tt, t.start = tk->src + tk->pos, t.length = len;
    t.line = tk->stus.line, t.col = tk->stus.col;
    return t;
}