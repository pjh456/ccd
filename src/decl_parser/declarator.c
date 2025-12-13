#include "decl_parser_impl/declarator.h"
#include "decl_parser_impl/decl_specifier_impl/decl_param.h"
#include "vector.h"
#include "utils.h"
#include <stdlib.h>

Declarator *make_identifier_declarator(char *name)
{
    Declarator *decl = malloc(sizeof(*decl));
    decl->type = DRT_IDENT;

    decl->ident.name = str_clone(name);

    return decl;
}

Declarator *make_pointer_declarator(Declarator *inner, unsigned qualifier)
{
    Declarator *decl = malloc(sizeof(*decl));
    decl->type = DRT_POINTER;

    decl->pointer.inner = inner;
    decl->pointer.qualifiers = qualifier;

    return decl;
}

Declarator *make_array_declarator(Declarator *inner, size_t length)
{
    Declarator *decl = malloc(sizeof(*decl));
    decl->type = DRT_ARRAY;

    decl->array.inner = inner;
    decl->array.length = length;

    return decl;
}

Declarator *make_function_declarator(Declarator *inner, Vector *params, int is_variadic)
{
    Declarator *decl = malloc(sizeof(*decl));
    decl->type = DRT_FUNCTION;

    decl->function.inner = inner;
    decl->function.params = params;
    decl->function.is_variadic = is_variadic;

    return decl;
}

void declarator_free(Declarator *decl)
{
    if (!decl)
        return;
    switch (decl->type)
    {
    case DRT_POINTER:
        declarator_free(decl->pointer.inner);
        break;
    case DRT_ARRAY:
        declarator_free(decl->array.inner);
        break;
    case DRT_FUNCTION:
        declarator_free(decl->function.inner);
        if (decl->function.params)
        {
            for (size_t idx = 0; idx < decl->function.params->size; ++idx)
                decl_param_free(*((DeclParam **)vector_get(decl->function.params, idx)));
            vector_free(decl->function.params);
        }
        break;
    case DRT_IDENT:
        if (decl->ident.name)
            free(decl->ident.name);
    default:
        break;
    }
    free(decl);
}