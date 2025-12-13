#include "decl_parser_impl/decl_specifier_impl/decl_param.h"
#include "decl_parser_impl/decl_specifier.h"
#include "decl_parser_impl/declarator.h"
#include "utils.h"
#include <stdlib.h>

DeclParam *make_decl_param(char *name, DeclSpecifier *spec, Declarator *decl)
{
    if (!spec || !decl)
        return NULL;
    DeclParam *dp = malloc(sizeof(*dp));

    dp->name = str_clone(name);
    dp->spec = spec;
    dp->decl = decl;

    return dp;
}

void decl_param_free(DeclParam *dp)
{
    if (!dp)
        return;

    if (dp->name)
        free(dp->name);
    decl_specifier_free(dp->spec);
    declarator_free(dp->decl);
    free(dp);
}