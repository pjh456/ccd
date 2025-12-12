#include "unit_scanner_impl/statement_unit.h"
#include "unit_scanner_impl/statement_unit_impl.h"
#include "unit_scanner_impl/statement_unit_break_impl.h"
#include "vector.h"
#include <stdlib.h>

StatementUnit *make_continue_statement_unit(Vector *tokens)
{
    if (!tokens)
        return NULL;
    StatementUnit *unit = calloc(1, sizeof(*unit));
    unit->type = SUT_CONTINUE;

    unit->tokens = tokens;

    return unit;
}

StatementUnit *make_break_statement_unit(Vector *tokens)
{
    if (!tokens)
        return NULL;
    StatementUnit *unit = calloc(1, sizeof(*unit));
    unit->type = SUT_BREAK;

    unit->tokens = tokens;

    return unit;
}

StatementUnit *make_return_statement_unit(Vector *tokens, StatementUnit *expr)
{
    if (!tokens)
        return NULL;
    StatementUnit *unit = calloc(1, sizeof(*unit));
    unit->type = SUT_RETURN;

    unit->tokens = tokens;
    unit->return_stmt.expr = expr;

    return unit;
}

void statement_unit_continue_free(StatementUnit *unit)
{
    if (!unit || unit->type != SUT_CONTINUE)
        return;
    statement_unit_free_tokens(unit->tokens);
    free(unit);
}

void statement_unit_break_free(StatementUnit *unit)
{
    if (!unit || unit->type != SUT_BREAK)
        return;
    statement_unit_free_tokens(unit->tokens);
    free(unit);
}

void statement_unit_return_free(StatementUnit *unit)
{
    if (!unit || unit->type != SUT_RETURN)
        return;
    statement_unit_free_tokens(unit->tokens);
    statement_unit_free(unit->return_stmt.expr);
    free(unit);
}