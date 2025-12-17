#include "decl_parser_impl/scope.h"
#include "hash_map.h"
#include <stdlib.h>

Scope *scope_enter(Scope *parent, ScopeType type, size_t count)
{
    Scope *scope = malloc(sizeof(*scope));

    scope->parent = parent;
    scope->type = type;
    scope->idents = make_hash_map(count);
    scope->tags = make_hash_map(count);
    scope->labels = make_hash_map(count);

    return scope;
}

void scope_free(Scope *scope)
{
    if (!scope)
        return;
    hash_map_free(scope->idents);
    hash_map_free(scope->tags);
    hash_map_free(scope->labels);
    free(scope);
}

Symbol *identifier_lookup(Scope *scope, const char *name)
{
    HashEntry *entry = NULL;
    for (Scope *cur = scope; cur && (!entry); cur = cur->parent)
        if (cur->idents)
            entry = hash_map_find(cur->idents, name);
    return entry ? (*(Symbol **)entry->value) : NULL;
}

Symbol *tag_lookup(Scope *scope, const char *name)
{
    HashEntry *entry = NULL;
    for (Scope *cur = scope; cur && (!entry); cur = cur->parent)
        if (cur->tags)
            entry = hash_map_find(cur->tags, name);
    return entry ? (*(Symbol **)entry->value) : NULL;
}

Symbol *label_lookup(Scope *scope, const char *name)
{
    HashEntry *entry = NULL;
    for (Scope *cur = scope; cur && (!entry); cur = cur->parent)
        if (cur->labels)
            entry = hash_map_find(cur->labels, name);
    return entry ? (*(Symbol **)entry->value) : NULL;
}