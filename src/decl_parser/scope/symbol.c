#include "decl_parser_impl/scope_impl/symbol.h"
#include "decl_parser_impl/scope.h"
#include "decl_parser_impl/decl_specifier.h"
#include "decl_parser_impl/declarator.h"
#include "utils.h"
#include <stdlib.h>

Symbol *make_symbol(
    Scope *scope,
    SymbolType type,
    DeclSpecifier *spec,
    Declarator *decl)
{
    if (!scope || !spec)
        return NULL;
    Symbol *symbol = malloc(sizeof(*symbol));

    symbol->type = type;
    symbol->scope = scope;
    symbol->spec = spec;
    symbol->decl = decl;
    if (decl && decl->name)
        symbol->name = str_clone(decl->name);
    else
        symbol->name = NULL;

    return symbol;
}

void symbol_free(Symbol *symbol)
{
    if (!symbol)
        return;

    if (symbol->name)
        free(symbol->name);
    scope_free(symbol->scope);
    decl_specifier_free(symbol->spec);
    declarator_free(symbol->decl);
    free(symbol);
}