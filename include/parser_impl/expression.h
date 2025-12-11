#pragma once

#include "../tokenizer_impl/token.h"
#include "c_type_info.h"

typedef enum
{
    EXPR_LITERAL, // 123, "abc"

    EXPR_IDENTIFIER, // a
    EXPR_CAST,       // (int)a

    EXPR_ASSIGN, // a = b, a += b ...
    EXPR_UNARY,  // ++a, a--, -a, !a ...
    EXPR_BINARY, // a + b, a * b, ...

    EXPR_CALL,   // f(a, b)
    EXPR_SIZEOF, // sizeof(int)

    EXPR_SUBSCRIPT,  // a[b]
    EXPR_MEMBER,     // a.b
    EXPR_PTR_MEMBER, // a->b

    EXPR_CONDITIONAL, // a ? b : c
    EXPR_COMMA,       // a , b
    EXPR_PAREN,       // (a)

    EXPR_UNKNOWN
} ExprType;

typedef struct
{
    ExprType type;
    Token token;
    union
    {
        struct
        {
            CTypeInfo *type_info;
            union
            {
                signed char signed_char_v;
                unsigned char unsigned_char_v;

                signed short int signed_short_v;
                unsigned short int unsigned_short_v;

                signed int signed_int_v;
                unsigned int unsigned_int_v;

                signed long int signed_long_v;
                unsigned long int unsigned_long_v;

                signed long long int signed_long_long_v;
                unsigned long long int unsigned_long_long_v;

                float float_v;
                double double_v;
                long double long_double_v;

                char *string_v;
            } data;
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
} Expression;

Expression *make_literal(CTypeInfo *cti, void *data);

void expression_free(Expression *expr);
