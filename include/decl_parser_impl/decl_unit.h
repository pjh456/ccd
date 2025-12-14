#pragma once

typedef struct Vector Vector;
typedef struct StatementUnit StatementUnit;
typedef struct DeclSpecifier DeclSpecifier;
typedef struct Declarator Declarator;
typedef struct DeclUnit DeclUnit;
typedef struct DeclInitializer DeclInitializer;

typedef enum
{
    DUT_DELARATION, // int a = 1;
    DUT_EXPRESSION, // 1 + 2
    DUT_STATEMENT,  // others
} DeclUnitType;

struct DeclUnit
{
    DeclUnitType type;
    union
    {
        struct
        {
            DeclSpecifier *spec;
            Vector *decls; // DeclInitializer
        } decl;

        struct
        {
            StatementUnit *origin;
        } expr;

        struct
        {
            StatementUnit *stmt;
        } stmt;
    };
};

DeclUnit *make_declaration_decl_unit(DeclSpecifier *spec, Vector *decls);
DeclUnit *make_expression_decl_unit(StatementUnit *origin);
DeclUnit *make_statement_decl_unit(StatementUnit *stmt);

void decl_unit_free(DeclUnit *unit);

void print_decl_unit(DeclUnit *unit);
