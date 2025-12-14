#include "decl_parser_impl/decl_unit.h"
#include "decl_parser_impl/decl_unit_impl/decl_unit_impl.h"
#include "decl_parser_impl/declarator.h"
#include "decl_parser_impl/declarator_impl/decl_initializer.h"
#include "decl_parser_impl/declarator_impl/declarator_impl.h"
#include "decl_parser_impl/decl_specifier.h"
#include "decl_parser_impl/decl_specifier_impl/decl_specifier_impl.h"
#include "unit_scanner_impl/statement_unit.h"
#include "unit_scanner_impl/statement_unit_impl/statement_unit_impl.h"
#include "vector.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>

DeclUnit *make_declaration_decl_unit(DeclSpecifier *spec, Vector *decls)
{
    if (!spec || !decls)
        return NULL;
    DeclUnit *unit = malloc(sizeof(*unit));
    unit->type = DUT_DELARATION;

    unit->decl.spec = spec;
    unit->decl.decls = decls;

    return unit;
}

DeclUnit *make_expression_decl_unit(StatementUnit *origin)
{
    if (!origin)
        return NULL;
    DeclUnit *unit = malloc(sizeof(*unit));
    unit->type = DUT_EXPRESSION;

    unit->expr.origin = origin;

    return unit;
}

DeclUnit *make_statement_decl_unit(StatementUnit *stmt)
{
    if (!stmt)
        return NULL;
    DeclUnit *unit = malloc(sizeof(*unit));
    unit->type = DUT_STATEMENT;

    unit->stmt.stmt = stmt;

    return unit;
}

void decl_unit_declaration_free(DeclUnit *unit)
{
    if (!unit || unit->type != DUT_DELARATION)
        return;
    decl_specifier_free(unit->decl.spec);
    if (unit->decl.decls)
    {
        for (size_t idx = 0; idx < unit->decl.decls->size; ++idx)
            decl_initializer_free(*((DeclInitializer **)vector_get(unit->decl.decls, idx)));
        vector_free(unit->decl.decls);
    }
    free(unit);
}

void decl_unit_expression_free(DeclUnit *unit)
{
    if (!unit || unit->type != DUT_EXPRESSION)
        return;
    statement_unit_free(unit->expr.origin);
    free(unit);
}

void decl_unit_statement_free(DeclUnit *unit)
{
    if (!unit || unit->type != DUT_STATEMENT)
        return;
    statement_unit_free(unit->stmt.stmt);
    free(unit);
}

void decl_unit_free(DeclUnit *unit)
{
    if (!unit)
        return;
    switch (unit->type)
    {
    case DUT_DELARATION:
        decl_unit_declaration_free(unit);
        break;
    case DUT_EXPRESSION:
        decl_unit_expression_free(unit);
        break;
    case DUT_STATEMENT:
        decl_unit_statement_free(unit);
        break;
    default:
        free(unit);
        return;
    }
}

void print_decl_unit(DeclUnit *du)
{
    print_decl_unit_impl(du, 0);
}

void print_decl_unit_impl(DeclUnit *du, int indent)
{
    if (!du)
    {
        print_indent(indent);
        printf("<null DeclUnit>\n");
        return;
    }

    switch (du->type)
    {
    case DUT_DELARATION:
    {
        print_indent(indent);
        printf("DeclUnit: Declaration\n");

        /* === Specifier === */
        print_indent(indent + 2);
        printf("Specifier:\n");
        print_decl_specifier_impl(du->decl.spec, indent + 4);

        /* === Declarators === */
        if (!du->decl.decls || du->decl.decls->size == 0)
        {
            print_indent(indent + 2);
            printf("Declarators: <none>\n");
            break;
        }

        print_indent(indent + 2);
        printf("Declarators (%zu):\n", du->decl.decls->size);

        for (size_t i = 0; i < du->decl.decls->size; i++)
        {
            DeclInitializer *di =
                *((DeclInitializer **)vector_get(du->decl.decls, i));

            print_indent(indent + 4);
            printf("Declarator #%zu:\n", i);

            /* Declarator */
            print_indent(indent + 6);
            printf("Pattern:\n");
            print_declarator_impl(di->decl, indent + 8);

            /* Initializer */
            print_indent(indent + 6);
            if (!di->init)
            {
                printf("Initializer: <none>\n");
            }
            else
            {
                printf("Initializer:\n");

                /* initializer 理论上只能是 expression */
                switch (di->init->type)
                {
                case DUT_EXPRESSION:
                    print_statement_unit_impl(
                        di->init->expr.origin,
                        indent + 8, 0);
                    break;

                case DUT_STATEMENT:
                    print_statement_unit_impl(
                        di->init->stmt.stmt,
                        indent + 8, 0);
                    break;

                case DUT_DELARATION:
                    print_indent(indent + 8);
                    printf("<unexpected declaration initializer>\n");
                    break;
                }
            }
        }
        break;
    }

    case DUT_EXPRESSION:
        print_indent(indent);
        printf("DeclUnit: Expression\n");
        print_statement_unit_impl(du->expr.origin, indent + 2, 0);
        break;

    case DUT_STATEMENT:
        print_indent(indent);
        printf("DeclUnit: Statement\n");
        print_statement_unit_impl(du->stmt.stmt, indent + 2, 0);
        break;
    }
}
