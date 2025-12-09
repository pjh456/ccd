#pragma once

#include "../tokenizer_impl/token.h"
#include "../vector.h"

typedef enum
{
    AST_IDENTIFIER, // x_a
    AST_NUMBER,     // 123
    AST_CHARACTER,  // 'a'
    AST_STRING,     // "xx"

    AST_NOT,         // !
    AST_TILDE,       // ~
    AST_AND,         // &
    AST_OR,          // |
    AST_XOR,         // ^
    AST_LEFT_SHIFT,  // <<
    AST_RIGHT_SHIFT, // >>

    AST_PLUS,   // +
    AST_MINUS,  // -
    AST_STAR,   // *
    AST_DIV,    // /
    AST_MOD,    // %
    AST_ASSIGN, // =

    AST_LESS,          // <
    AST_GREATER,       // >
    AST_EQUAL,         // ==
    AST_NOT_EQUAL,     // !=
    AST_LESS_EQUAL,    // <=
    AST_GREATER_EQUAL, // >=
    AST_AND_AND,       // &&
    AST_OR_OR,         // ||

    AST_AND_ASSIGN,         // &=
    AST_OR_ASSIGN,          // |=
    AST_XOR_ASSIGN,         // ^=
    AST_LEFT_SHIFT_ASSIGN,  // <<=
    AST_RIGHT_SHIFT_ASSIGN, // <<=

    AST_PLUS_ASSIGN,  // +=
    AST_MINUS_ASSIGN, // -=
    AST_MUL_ASSIGN,   // *=
    AST_DIV_ASSIGN,   // /=
    AST_MOD_ASSIGN,   // %=
    AST_INC,          // ++
    AST_DEC,          // --

    AST_VAR_DECL,   // int a;
    AST_VAR_ASSIGN, // a = 1;
    AST_FUNC_DECL,  // int foo(int x);
    AST_FUNC_IMPL,  // int foo(int x) {...}

    AST_BLOCK, // { ... }
    // AST_EXPRESSION, // a + 1
    AST_IF,       // if
    AST_WHILE,    // while
    AST_DO_WHILE, // do-while
    AST_FOR,      // for
    AST_RETURN,   // return
    AST_BREAK,    // break
    AST_CONTINUE, // continue
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
            ASTNode *lhs;
            ASTNode *rhs;
        } bin;

        long val;

        struct
        {
            char *name;
            size_t len;
        } var;

        struct
        {
            Vector *stmts;
        } block;

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
        } loop;

        struct
        {
            ASTNode *expr;
        } expr;

        struct
        {
            char *name;
            size_t len;
            ASTNode *init;
        } decl;
    };
};

// ASTNode *ast_node_new(NodeType nt, Token *t);
// void ast_node_free(ASTNode *node);

// void print_ast(ASTNode *node, int indent);