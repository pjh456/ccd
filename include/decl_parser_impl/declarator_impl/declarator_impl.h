#pragma once

typedef struct Declarator Declarator;
typedef struct DeclInitializer DeclInitializer;

void print_declarator_impl(Declarator *d, int indent);

void print_decl_initializer_impl(DeclInitializer *di, int indent);