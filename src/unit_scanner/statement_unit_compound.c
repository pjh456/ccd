#include "unit_scanner_impl/statement_unit.h"
#include "unit_scanner_impl/statement_unit_impl.h"
#include "unit_scanner_impl/statement_unit_compound_impl.h"
#include "vector.h"
#include <stdlib.h>

StatementUnit *make_compound_statement_unit(Vector *tokens, Vector *units)
{
    if (!tokens)
        return NULL;
    StatementUnit *unit = calloc(1, sizeof(*unit));
    unit->type = SUT_COMPOUND;

    unit->tokens = tokens;
    unit->compound_stmt.units = units;

    return unit;
}

StatementUnit *make_empty_statement_unit(Vector *tokens)
{
    if (!tokens)
        return NULL;
    StatementUnit *unit = calloc(1, sizeof(*unit));
    unit->type = SUT_EMPTY;

    unit->tokens = tokens;

    return unit;
}

void statement_unit_compound_free(StatementUnit *unit)
{
    if (!unit || unit->type != SUT_COMPOUND)
        return;
    statement_unit_free_tokens(unit->tokens);
    if (unit->compound_stmt.units)
    {
        for (size_t idx = 0; idx < unit->compound_stmt.units->size; ++idx)
            statement_unit_free(*((StatementUnit **)vector_get(unit->compound_stmt.units, idx)));
        vector_free(unit->compound_stmt.units);
    }
    free(unit);
}

void statement_unit_empty_free(StatementUnit *unit)
{
    if (!unit || unit->type != SUT_EMPTY)
        return;
    statement_unit_free_tokens(unit->tokens);
    free(unit);
}