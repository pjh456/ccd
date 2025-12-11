#include "parser_impl/declaration.h"
#include "parser_impl/c_type_info.h"
#include "parser_impl/expression.h"

#include <stdlib.h>
#include <string.h>

Declaration *make_declaration(CTypeInfo *cti, char *name, Expression *expr)
{
    if (!cti)
        return NULL;

    Declaration *decl = malloc(sizeof(*decl));
    decl->type_info = cti;
    if (name)
    {
        decl->name = malloc(strlen(name) + 1);
        strcpy(decl->name, name);
    }
    else
        decl->name = NULL;

    decl->init = expr;
    return decl;
}

void declaration_free(Declaration *decl)
{
    if (!decl)
        return;
    c_type_info_free(decl->type_info);
    if (decl->name)
        free(decl->name);
    expression_free(decl->init);
    free(decl);
}