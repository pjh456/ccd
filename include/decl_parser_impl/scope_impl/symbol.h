#pragma once

typedef struct Declarator Declarator;
typedef struct DeclSpecifier DeclSpecifier;
typedef enum SymbolType SymbolType;
typedef struct Symbol Symbol;
typedef struct Scope Scope;

enum SymbolType
{
    SYT_VAR,
    SYT_FUNC,

    SYT_TYPEDEF,
    SYT_STRUCT,
    SYT_UNION,
    SYT_ENUM,
    SYT_ENUM_ITEM,

    SYT_LABEL
};

struct Symbol
{
    char *name;
    SymbolType type;

    Scope *scope;
    DeclSpecifier *spec;
    Declarator *decl;
};

Symbol *make_symbol(
    Scope *scope,
    SymbolType type,
    DeclSpecifier *spec,
    Declarator *decl);

void symbol_free(Symbol *symbol);