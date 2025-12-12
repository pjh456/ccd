#include "unit_scanner_impl/statement_unit.h"
#include "unit_scanner_impl/statement_unit_impl/statement_unit_impl.h"
#include "unit_scanner_impl/statement_unit_impl/statement_unit_declaration_impl.h"
#include "vector.h"
#include <stdlib.h>

StatementUnit *make_declaration_statement_unit(Vector *tokens)
{
    if (!tokens)
        return NULL;
    StatementUnit *unit = calloc(1, sizeof(*unit));
    unit->type = SUT_DECL;

    unit->tokens = tokens;

    return unit;
}

StatementUnit *make_expression_statement_unit(Vector *tokens)
{
    if (!tokens)
        return NULL;
    StatementUnit *unit = calloc(1, sizeof(*unit));
    unit->type = SUT_EXPR;

    unit->tokens = tokens;

    return unit;
}

void statement_unit_declaration_free(StatementUnit *unit)
{
    if (!unit || unit->type != SUT_DECL)
        return;
    statement_unit_free_tokens(unit->tokens);
    free(unit);
}

void statement_unit_expression_free(StatementUnit *unit)
{
    if (!unit || unit->type != SUT_EXPR)
        return;
    statement_unit_free_tokens(unit->tokens);
    free(unit);
}