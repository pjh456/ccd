#include "decl_parser_impl/declarator.h"
#include "decl_parser_impl/declarator_impl/declarator_impl.h"
#include "decl_parser_impl/declarator_impl/decl_param.h"
#include "decl_parser_impl/decl_specifier.h"
#include "decl_parser_impl/decl_specifier_impl/decl_specifier_impl.h"
#include "decl_parser_impl/decl_unit.h"
#include "vector.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>

Declarator *make_identifier_declarator(char *name)
{
    Declarator *decl = malloc(sizeof(*decl));
    decl->type = DRT_IDENT;

    decl->name = str_clone(name);

    return decl;
}

Declarator *make_pointer_declarator(Declarator *inner, unsigned qualifier)
{
    Declarator *decl = malloc(sizeof(*decl));
    decl->type = DRT_POINTER;

    decl->name = str_clone(inner ? inner->name : NULL);
    decl->pointer.inner = inner;
    decl->pointer.qualifiers = qualifier;

    return decl;
}

Declarator *make_array_declarator(Declarator *inner, DeclUnit *length)
{
    Declarator *decl = malloc(sizeof(*decl));
    decl->type = DRT_ARRAY;

    decl->name = str_clone(inner ? inner->name : NULL);
    decl->array.inner = inner;
    decl->array.length = length;

    return decl;
}

Declarator *make_function_declarator(Declarator *inner, Vector *params, int is_variadic)
{
    Declarator *decl = malloc(sizeof(*decl));
    decl->type = DRT_FUNCTION;

    decl->name = str_clone(inner ? inner->name : NULL);
    decl->function.inner = inner;
    decl->function.params = params;
    decl->function.is_variadic = is_variadic;

    return decl;
}

Declarator *make_group_declarator(Declarator *inner)
{
    Declarator *decl = malloc(sizeof(*decl));
    decl->type = DRT_GROUP;

    decl->name = str_clone(inner ? inner->name : NULL);
    decl->function.inner = inner;

    return decl;
}

void declarator_free(Declarator *decl)
{
    if (!decl)
        return;
    if (decl->name)
        free(decl->name);
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
    case DRT_GROUP:
        declarator_free(decl->group.inner);
        break;
    case DRT_IDENT:
    default:
        break;
    }
    free(decl);
}

void print_declarator(Declarator *d)
{
    print_declarator_impl(d, 0);
}

void print_declarator_impl(Declarator *d, int indent)
{
    if (!d)
    {
        print_indent(indent);
        printf("<null Declarator>\n");
        return;
    }

    switch (d->type)
    {
    case DRT_IDENT:
        print_indent(indent);
        printf("Declarator: IDENT \"%s\"\n",
               d->name ? d->name : "<anonymous>");
        break;

    case DRT_POINTER:
        print_indent(indent);
        printf("Declarator: POINTER\n");

        if (d->pointer.qualifiers)
        {
            print_indent(indent + 2);
            printf("Qualifiers:");
            print_decl_qualifiers(d->pointer.qualifiers);
            printf("\n");
        }

        print_indent(indent + 2);
        printf("Inner:\n");
        print_declarator_impl(d->pointer.inner, indent + 4);
        break;

    case DRT_ARRAY:
        print_indent(indent);
        printf("Declarator: ARRAY\n");

        print_indent(indent + 2);
        printf("Length: ");
        // if (d->array.length)
        //     printf("%zu\n", d->array.length);
        // else
        printf("<unspecified>\n");

        print_indent(indent + 2);
        printf("Element:\n");
        print_declarator_impl(d->array.inner, indent + 4);
        break;

    case DRT_FUNCTION:
        print_indent(indent);
        printf("Declarator: FUNCTION\n");

        print_indent(indent + 2);
        printf("Return:\n");
        print_declarator_impl(d->function.inner, indent + 4);

        print_indent(indent + 2);
        printf("Parameters:\n");

        if (!d->function.params || d->function.params->size == 0)
        {
            print_indent(indent + 4);
            printf("(none)\n");
        }
        else
        {
            for (size_t i = 0; i < d->function.params->size; i++)
            {
                DeclParam *p = *((DeclParam **)vector_get(d->function.params, i));

                print_indent(indent + 4);
                printf("Param %zu: %s\n",
                       i,
                       p->name ? p->name : "<anonymous>");

                print_indent(indent + 6);
                printf("Specifier:\n");
                print_decl_specifier_impl(p->spec, indent + 8);

                print_indent(indent + 6);
                printf("Declarator:\n");
                print_declarator_impl(p->decl, indent + 8);
            }
        }

        if (d->function.is_variadic)
        {
            print_indent(indent + 4);
            printf("Variadic: yes\n");
        }
        break;

    case DRT_GROUP:
        print_indent(indent);
        printf("Declarator: GROUP\n");

        print_indent(indent + 2);
        printf("Inner:\n");

        if (d->group.inner)
            print_declarator_impl(d->group.inner, indent + 4);
        else
        {
            print_indent(indent + 4);
            printf("<none>\n");
        }
    }
}
