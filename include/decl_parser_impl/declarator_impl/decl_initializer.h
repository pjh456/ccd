#pragma once

typedef struct DeclInitializer DeclInitializer;
typedef struct Declarator Declarator;
typedef struct DeclUnit DeclUnit;

struct DeclInitializer
{
    Declarator *decl;
    DeclUnit *init;
};

DeclInitializer *make_decl_initializer(Declarator *decl, DeclUnit *init);

void decl_initializer_free(DeclInitializer *di);

void print_decl_initializer(DeclInitializer *di);