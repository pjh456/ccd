#pragma once

#include "../tokenizer_impl/token.h"
#include "literal_value.h"
#include "c_type_info.h"

typedef struct Expression Expression;

typedef enum
{
    EXPR_LITERAL, // 123, "abc"

    EXPR_IDENTIFIER, // a
    EXPR_CAST,       // (int)a
    EXPR_ASSIGN,     // a = b, a += b ...
    EXPR_UNARY,      // ++a, a--, -a, !a ...
    EXPR_BINARY,     // a + b, a * b, ...
    EXPR_CALL,       // f(a, b)

    EXPR_SUBSCRIPT,  // a[b]
    EXPR_MEMBER,     // a.b
    EXPR_PTR_MEMBER, // a->b

    EXPR_CONDITIONAL, // a ? b : c
    EXPR_COMMA,       // a , b
    EXPR_PAREN,       // (a)
} ExprType;

struct Expression
{
    ExprType type;
    Token token;
    union
    {
        struct
        {
            LiteralValue v;
        } literal;

        struct
        {
            char *name;
        } ident;

        struct
        {
            CTypeInfo *typeinfo;
            Expression *expr;
        } cast;

        struct
        {
            Expression *operand;
            int op, prefix;
        } unary;

        struct
        {
            Expression *lhs, *rhs;
        } binary;

        struct
        {
            Expression *lhs, *rhs;
        } assign;

        struct
        {
            Expression *callee;
            Expression **args;
            int count;
        } call;

        struct
        {
            Expression *base, *index;
        } subscript;

        struct
        {
            Expression *cond, *then_expr, *else_expr;
        } conditional;

        struct
        {
            Expression **exprs;
            int count;
        } comma;
    };
};