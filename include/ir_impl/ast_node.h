#pragma once

#include "../tokenizer_impl/token.h"
#include "../vector.h"

typedef enum
{
    AST_IDENTIFIER, // x_a
    AST_NUMBER,     // 123
    AST_CHARACTER,  // 'a'
    AST_STRING,     // "xx"

    AST_NOT,         // !x
    AST_TILDE,       // ~x
    AST_AND,         // x & y
    AST_OR,          // x | y
    AST_XOR,         // x ^ y
    AST_LEFT_SHIFT,  // x << y
    AST_RIGHT_SHIFT, // x >> y

    AST_PLUS,  // x + y
    AST_MINUS, // x - y
    AST_STAR,  // x * y
    AST_DIV,   // x / y
    AST_MOD,   // x % y

    AST_LESS,          // x < y
    AST_GREATER,       // x > y
    AST_EQUAL,         // x == y
    AST_NOT_EQUAL,     // x != y
    AST_LESS_EQUAL,    // x <= y
    AST_GREATER_EQUAL, // x >= y
    AST_AND_AND,       // x && y
    AST_OR_OR,         // x || y

    AST_AND_ASSIGN,         // x &= y
    AST_OR_ASSIGN,          // x |= y
    AST_XOR_ASSIGN,         // x ^= y
    AST_LEFT_SHIFT_ASSIGN,  // x <<= y
    AST_RIGHT_SHIFT_ASSIGN, // x <<= y

    AST_PLUS_ASSIGN,  // x += y
    AST_MINUS_ASSIGN, // x -= y
    AST_MUL_ASSIGN,   // x *= y
    AST_DIV_ASSIGN,   // x /= y
    AST_MOD_ASSIGN,   // x %= y
    AST_INC,          // x ++, ++ x
    AST_DEC,          // x --, -- x

    AST_VAL2PTR, // &x
    AST_PTR2VAL, // *x

    AST_VAR_DECL,  // int x;
    AST_ASSIGN,    // x = 1
    AST_FUNC_DECL, // int foo(int x);
    AST_FUNC_IMPL, // int foo(int x) {...}

    AST_BLOCK,      // { ... }
    AST_EXPRESSION, // a + 1
    AST_IF,         // if
    AST_SWITCH,     // switch
    AST_CASE,       // case
    AST_WHILE,      // while
    AST_DO_WHILE,   // do-while
    AST_FOR,        // for
    AST_RETURN,     // return
    AST_BREAK,      // break
    AST_CONTINUE,   // continue
    AST_LABEL,      // label
    AST_GOTO,       // goto
} NodeType;

typedef struct ASTNode ASTNode;

struct ASTNode
{
    NodeType type;
    Token *token;

    union
    {
        struct
        {
            Vector *stmts;
        } block;

        struct
        {
            ASTNode *expr;
        } expr;

        char *var_name;

        struct
        {
            ASTNode *lhs;
            ASTNode *rhs;
        } bin;

        struct
        {
            ASTNode *var;
            ASTNode *expr
        } assign;

        struct
        {
            char *type;
            char *name;
            size_t len;
            ASTNode *init;
        } var_decl;

        struct
        {
            char *name;
            size_t len;
            Vector *params;
        } func_decl;

        struct
        {
            ASTNode *decl;
            ASTNode *body;
        } func_impl;

        struct
        {
            ASTNode *cond;
            ASTNode *then;
            ASTNode *els;
        } if_stmt;

        struct
        {
            ASTNode *init;
            ASTNode *cond;
            ASTNode *inc;
            ASTNode *body;
        } for_stmt;

        struct
        {
            ASTNode *cond;
            ASTNode *body;
        } while_stmt;

        struct
        {
            ASTNode *cond;
            ASTNode *body;
        } do_while_stmt;
    };
};

ASTNode *ast_node_new(NodeType nt, Token *t);
void ast_node_free(ASTNode *node);

// void print_ast(ASTNode *node, int indent);