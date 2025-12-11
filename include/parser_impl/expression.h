#pragma once

#include "../tokenizer_impl/token.h"

typedef struct Vector Vector;
typedef struct CTypeInfo CTypeInfo;
typedef struct Expression Expression;

typedef enum
{
    OP_ASSIGN, // a = b

    // === 复合赋值位运算符 (&=, <<= 等) ===
    OP_AND_ASSIGN,         // a &= b
    OP_OR_ASSIGN,          // a |= b
    OP_XOR_ASSIGN,         // a ^= b
    OP_LEFT_SHIFT_ASSIGN,  // a <<= b
    OP_RIGHT_SHIFT_ASSIGN, // a <<= b

    // === 复合赋值算数运算符 (+=, -= 等) ===
    OP_PLUS_ASSIGN,  // a += b
    OP_MINUS_ASSIGN, // a -= b
    OP_MUL_ASSIGN,   // a *= b
    OP_DIV_ASSIGN,   // a /= b
    OP_MOD_ASSIGN,   // a %= b
} AssignOperator;

typedef enum
{
    // === 位运算符 ===
    OP_NOT,   // ! (逻辑非)
    OP_TILDE, // ~ (按位取反)

    // 数字运算
    OP_NEG, // 取反

    // === 自增自减 ===
    OP_INC, // ++
    OP_DEC, // --
} UnaryOperator;

typedef enum
{
    // === 比较/逻辑运算符 ===
    OP_LESS,          // <
    OP_GREATER,       // >
    OP_EQUAL,         // ==
    OP_NOT_EQUAL,     // !=
    OP_LESS_EQUAL,    // <=
    OP_GREATER_EQUAL, // >=
    OP_AND_AND,       // &&
    OP_OR_OR,         // ||

    // === 位运算符 ===
    OP_AND,         // &
    OP_OR,          // |
    OP_XOR,         // ^
    OP_LEFT_SHIFT,  // <<
    OP_RIGHT_SHIFT, // >>

    // === 算术运算符 ===
    OP_PLUS,  // +
    OP_MINUS, // -
    OP_STAR,  // *
    OP_DIV,   // /
    OP_MOD,   // %
} BinaryOperator;

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

struct Expression
{
    ExprType type;
    Token token;
    CTypeInfo *type_info;
    union
    {
        struct
        {
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
            CTypeInfo *type_info;
            Expression *expr;
        } cast;

        struct
        {
            Expression *lhs, *rhs;
            AssignOperator op;
        } assign;

        struct
        {
            Expression *expr;
            UnaryOperator op;
            int is_prefix;
        } unary;

        struct
        {
            Expression *lhs, *rhs;
            BinaryOperator op;
        } binary;

        struct
        {
            Expression *func;
            Vector *args; // Expressions
        } call;

        struct
        {
            Expression *expr;
        } sizeof_call;

        struct
        {
            Expression *base, *index;
        } subscript;

        struct
        {
            Expression *base, *mem;
        } member;

        struct
        {
            Expression *cond, *then_expr, *else_expr;
        } conditional;

        struct
        {
            Vector *exprs; // Expressions
        } comma;

        struct
        {
            Expression *expr;
        } paren;
    };
};

Expression *make_expression_literal(CTypeInfo *cti, void *data);
Expression *make_expression_identifier(char *name);
Expression *make_expression_cast(CTypeInfo *cti, Expression *expr);
Expression *make_expression_assign(AssignOperator op, Expression *lhs, Expression *rhs);
Expression *make_expression_unary(UnaryOperator op, Expression *expr, int is_prefix);
Expression *make_expression_binary(BinaryOperator op, Expression *lhs, Expression *rhs);
Expression *make_expression_call(Expression *func, Vector *args);
Expression *make_expression_sizeof(Expression *expr);
Expression *make_expression_subscript(Expression *base, Expression *index);
Expression *make_expression_member(Expression *base, Expression *mem);
Expression *make_expression_ptr_member(Expression *base, Expression *mem);
Expression *make_expression_conditional(Expression *cond, Expression *then_expr, Expression *else_expr);
Expression *make_expression_comma(Vector *exprs);
Expression *make_expression_paren(Expression *expr);

void expression_free(Expression *expr);

void print_expression(Expression *expr);
