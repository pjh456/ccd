#pragma once

#include <stddef.h>

typedef struct Vector Vector;
typedef struct DeclUnit DeclUnit;
typedef struct Declarator Declarator;

typedef enum
{
    DRT_IDENT,    // 叶子：标识符
    DRT_POINTER,  // *
    DRT_ARRAY,    // []
    DRT_FUNCTION, // x()
    DRT_GROUP,    // (x)
} DeclaratorType;

struct Declarator
{
    DeclaratorType type;
    char *name;
    union
    {
        struct
        {
            Declarator *inner;
            unsigned qualifiers; // const / volatile / restrict
        } pointer;

        struct
        {
            Declarator *inner;
            DeclUnit *length; // 可为 0 / UNKNOWN
        } array;

        struct
        {
            Declarator *inner;
            Vector *params; // DeclParam*
            int is_variadic;
        } function;

        struct
        {
            Declarator *inner;
        } group;
    };
};

Declarator *make_identifier_declarator(char *name);
Declarator *make_pointer_declarator(Declarator *inner, unsigned qualifier);
Declarator *make_array_declarator(Declarator *inner, DeclUnit *length);
Declarator *make_function_declarator(Declarator *inner, Vector *params, int is_variadic);
Declarator *make_group_declarator(Declarator *inner);

void declarator_free(Declarator *decl);

void print_declarator(Declarator *decl);