#pragma once

typedef struct Vector Vector;
typedef struct StatementUnit StatementUnit;

typedef enum
{
    SUT_COMPOUND, // { ... }
    SUT_EMPTY,    // ;

    SUT_DECL, // int a = 1;
    SUT_EXPR, // 1 + 2

    SUT_IF,      // if
    SUT_SWITCH,  // switch
    SUT_CASE,    // case
    SUT_DEFAULT, // default

    SUT_WHILE,    // while { ... }
    SUT_DO_WHILE, // do { ... } while(cond);
    SUT_FOR,      // for(...; ...; ...) { ... }

    SUT_CONTINUE, // continue
    SUT_BREAK,    // break
    SUT_RETURN,   // return

    SUT_LABEL, // label:
    SUT_GOTO,  // goto label

} StatementUnitType;

struct StatementUnit
{
    Vector *tokens; // Tokens
    StatementUnitType type;

    union
    {
        struct
        {
            Vector *units; // StatementUnits
        } compound_stmt;

        struct
        {
            StatementUnit *cond;
            StatementUnit *then_body;
            StatementUnit *else_body;
        } if_stmt;

        struct
        {
            StatementUnit *expr;
            StatementUnit *body;
        } switch_stmt;

        struct
        {
            StatementUnit *expr;
        } case_stmt;

        struct
        {
            int dummy;
        } default_stmt;

        struct
        {
            StatementUnit *cond;
            StatementUnit *body;
        } while_stmt;

        struct
        {
            StatementUnit *body;
            StatementUnit *cond;
        } do_while_stmt;

        struct
        {
            StatementUnit *expr;
        } return_stmt;

        struct
        {
            char *name;
        } label_stmt;

        struct
        {
            char *name;
        } goto_stmt;
    };
};

StatementUnit *make_compound_statement_unit(Vector *tokens, Vector *units);
StatementUnit *make_empty_statement_unit(Vector *tokens);

StatementUnit *make_declaration_statement_unit(Vector *tokens);
StatementUnit *make_expression_statement_unit(Vector *tokens);

StatementUnit *make_if_statement_unit(
    Vector *tokens,
    StatementUnit *cond,
    StatementUnit *then_body,
    StatementUnit *else_body);
StatementUnit *make_switch_statement_unit(
    Vector *tokens,
    StatementUnit *expr,
    StatementUnit *body);
StatementUnit *make_case_statement_unit(Vector *tokens, StatementUnit *expr);
StatementUnit *make_default_statement_unit(Vector *tokens, int dummy);

StatementUnit *make_while_statement_unit();
StatementUnit *make_do_while_statement_unit();
StatementUnit *make_for_statement_unit();

StatementUnit *make_continue_statement_unit();
StatementUnit *make_break_statement_unit();
StatementUnit *make_return_statement_unit();

StatementUnit *make_label_statement_unit();
StatementUnit *make_goto_statement_unit();

void statement_unit_free(StatementUnit *unit);