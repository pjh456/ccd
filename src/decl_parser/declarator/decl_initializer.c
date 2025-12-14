#include "decl_parser_impl/declarator_impl/decl_initializer.h"
#include "decl_parser_impl/declarator.h"
#include "decl_parser_impl/decl_unit.h"
#include "decl_parser_impl/declarator_impl/declarator_impl.h"
#include "unit_scanner_impl/statement_unit_impl/statement_unit_impl.h"
#include "utils.h"
#include "vector.h"
#include <stdlib.h>
#include <stdio.h>

DeclInitializer *make_decl_initializer(Declarator *decl, DeclUnit *init)
{
    if (!decl || (init && init->type != DUT_EXPRESSION))
        return NULL;
    DeclInitializer *di = malloc(sizeof(*di));
    di->decl = decl;
    di->init = init;

    return di;
}

void decl_initializer_free(DeclInitializer *di)
{
    if (!di)
        return;
    declarator_free(di->decl);
    decl_unit_free(di->init);
    free(di);
}

void print_decl_initializer_impl(DeclInitializer *di, int indent);

void print_decl_initializer(DeclInitializer *di)
{
    print_decl_initializer_impl(di, 0);
}

void print_decl_initializer_impl(DeclInitializer *di, int indent)
{
    if (!di)
    {
        print_indent(indent);
        printf("<null initializer>\n");
        return;
    }

    print_indent(indent);
    printf("InitDeclarator:\n");

    /* declarator */
    print_indent(indent + 4);
    printf("Declarator:\n");
    print_declarator_impl(di->decl, indent + 8);

    /* initializer */
    if (!di->init)
    {
        print_indent(indent + 4);
        printf("Initializer: <none>\n");
        return;
    }

    print_indent(indent + 4);
    printf("Initializer:\n");

    /* 这里只打印 expression / statement 的原始形态 */
    switch (di->init->type)
    {
    case DUT_EXPRESSION:
        print_indent(indent + 8);
        printf("Expression (raw):\n");
        print_statement_unit_impl(di->init->expr.origin, indent + 12, 0);
        break;

    case DUT_STATEMENT:
        print_indent(indent + 8);
        printf("Statement (raw):\n");
        print_statement_unit_impl(di->init->stmt.stmt, indent + 12, 0);
        break;

    case DUT_DELARATION:
        print_indent(indent + 8);
        printf("Declaration (unexpected in initializer)\n");
        break;
    }
}