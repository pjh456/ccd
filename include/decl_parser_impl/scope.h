#pragma once

#include <stddef.h>

typedef struct Scope Scope;
typedef struct Symbol Symbol;
typedef struct HashMap HashMap;
typedef struct DeclSpecifier DeclSpecifier;
typedef struct Declarator Declarator;

typedef enum
{
    SYM_OBJECT,
    SYM_FUNCTION,
    SYM_TYPEDEF,
    SYM_ENUM_CONST,
    SYM_STRUCT_TAG,
    SYM_UNION_TAG,
    SYM_ENUM_TAG,
    SYM_FIELD
} ScopeType;

struct Scope
{
    Scope *parent;
    ScopeType type;

    HashMap *idents;
    HashMap *tags;
    HashMap *labels;
};

Scope *scope_enter(Scope *parent, ScopeType type, size_t count);

void scope_free(Scope *scope);

Symbol *identifier_lookup(Scope *scope, const char *name);
Symbol *tag_lookup(Scope *scope, const char *name);
Symbol *label_lookup(Scope *scope, const char *name);
