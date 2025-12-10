#pragma once

#include "declaration.h"

typedef struct Statement Statement;

typedef enum
{
    STMT_COMPOUND, // { ... }
    STMT_EXPR,     // expression;
    STMT_DECL,     // int a, b;

    STMT_IF,       // if (...) ... else ...
    STMT_WHILE,    // while (...)
    STMT_DO_WHILE, // do ... while
    STMT_FOR,      // for(...)

    STMT_BREAK,
    STMT_CONTINUE,
    STMT_RETURN, // return expr;

    STMT_SWITCH,
    STMT_CASE,
    STMT_DEFAULT,

    STMT_LABEL, // foo: ...
    STMT_GOTO
} StatementType;

struct Statement
{
    StatementType kind;

    union
    {
        // 1. 复合语句 { ... }
        struct
        {
            Statement **stmts;
            int stmt_count;
        } compound;

        // 2. 表达式语句
        struct
        {
            Expression *expr; // 可为 NULL (如 ";")
        } expr_stmt;

        // 3. 声明语句（变量声明）
        struct
        {
            Declaration **decls;
            int decl_count;
        } decl;

        // 4. if (...) ... else ...
        struct
        {
            Expression *cond;
            Statement *then_branch;
            Statement *else_branch;
        } if_stmt;

        // 5. while (...)
        struct
        {
            Expression *cond;
            Statement *body;
        } while_stmt;

        // 6. do ... while
        struct
        {
            Statement *body;
            Expression *cond;
        } do_while_stmt;

        // 7. for (...)
        struct
        {
            Statement *init;  // 可以是 decl 或 expr 或 NULL
            Expression *cond; // NULL 表示 true
            Expression *post; // NULL 表示无
            Statement *body;
        } for_stmt;

        // 8. return expr
        struct
        {
            Expression *expr; // 可为 NULL
        } return_stmt;

        // 9. break / continue
        struct
        {
            int dummy;
        } jump_stmt;

        // 10. switch(expr) { ... }
        struct
        {
            Expression *expr;
            Statement *body;
        } switch_stmt;

        // 11. case n: ...
        struct
        {
            long long value;
            Statement *body;
        } case_stmt;

        // 12. default: ...
        struct
        {
            Statement *body;
        } default_stmt;

        // 13. label: ...
        struct
        {
            char *label_name;
            Statement *stmt;
        } label_stmt;

        // 14. goto label;
        struct
        {
            char *label_name;
        } goto_stmt;
    };
};