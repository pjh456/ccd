#pragma once

typedef struct DeclParam DeclParam;
typedef struct DeclSpecifier DeclSpecifier;
typedef struct Declarator Declarator;

struct DeclParam
{
    DeclSpecifier *spec;
    Declarator *decl;
};

DeclParam *make_decl_param(DeclSpecifier *spec, Declarator *decl);

void decl_param_free(DeclParam *dp);