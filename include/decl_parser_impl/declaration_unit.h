#pragma once

typedef struct Vector Vector;
typedef struct StatementUnit StatementUnit;
typedef struct DeclSpecifier DeclSpecifier;
typedef struct Declarator Declarator;
typedef struct DeclarationUnit DeclarationUnit;

typedef enum
{
    DUT_DELARATION, // int a = 1;
    DUT_EXPRESSION, // 1 + 2
    DUT_STATEMENT,  // others
} DeclarationUnitType;

struct DeclarationUnit
{
    DeclarationUnitType type;
    union
    {
        struct
        {
            Vector *specifiers;  // DeclSpecifiers
            Vector *declarators; // Declarators
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