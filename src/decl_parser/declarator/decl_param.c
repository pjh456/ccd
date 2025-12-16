#include "decl_parser_impl/declarator.h"
#include "decl_parser_impl/declarator_impl/decl_param.h"
#include "decl_parser_impl/decl_specifier.h"
#include "decl_parser_impl/declarator.h"
#include "utils.h"
#include <stdlib.h>

DeclParam *make_decl_param(DeclSpecifier *spec, Declarator *decl)
{
    if (!spec)
        return NULL;
    DeclParam *dp = malloc(sizeof(*dp));

    if (decl)
        dp->name = str_clone(decl->name ? decl->name : NULL);
    else
        dp->name = NULL;
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