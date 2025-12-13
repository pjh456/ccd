#include "decl_parser_impl/decl_specifier.h"
#include <stdlib.h>

DeclSpecifier *make_decl_specifier(
    DeclBuiltinType builtin_type,
    DeclSUE sue_type,
    unsigned storages,
    unsigned func_specs,
    unsigned qualifiers,
    unsigned modifiers)
{
    DeclSpecifier *ds = calloc(1, sizeof(*ds));

    ds->builtin_type = builtin_type;
    ds->sue_type = sue_type;
    ds->storages = storages;
    ds->func_specifiers = func_specs;
    ds->qualifiers = qualifiers;
    ds->modifiers = modifiers;

    return ds;
}

void decl_specifier_free(DeclSpecifier *ds)
{
    if (!ds)
        return;
    switch (ds->sue_type)
    {
    case DSUE_STRUCT:
        break;
    case DSUE_UNION:
        break;
    case DSUE_ENUM:
        break;
    case DSUE_NONE:
    default:
        break;
    }
    free(ds);
}