#include "unit_scanner_impl/statement_unit.h"
#include "unit_scanner_impl/statement_unit_impl.h"
#include "unit_scanner_impl/statement_unit_loop_impl.h"
#include "vector.h"
#include <stdlib.h>

StatementUnit *make_while_statement_unit(
    Vector *tokens,
    StatementUnit *cond,
    StatementUnit *body)
{
    if (!tokens || !cond || !body)
        return NULL;
    StatementUnit *unit = calloc(1, sizeof(*unit));
    unit->type = SUT_WHILE;

    unit->tokens = tokens;
    unit->while_stmt.cond = cond;
    unit->while_stmt.body = body;

    return unit;
}

StatementUnit *make_do_while_statement_unit(
    Vector *tokens,
    StatementUnit *body,
    StatementUnit *cond)
{
    if (!tokens || !body || !cond)
        return NULL;
    StatementUnit *unit = calloc(1, sizeof(*unit));
    unit->type = SUT_DO_WHILE;

    unit->tokens = tokens;
    unit->do_while_stmt.body = body;
    unit->do_while_stmt.cond = cond;

    return unit;
}

StatementUnit *make_for_statement_unit(
    Vector *tokens,
    StatementUnit *init,
    StatementUnit *cond,
    StatementUnit *step,
    StatementUnit *body)
{
    if (!tokens || !body)
        return NULL;
    StatementUnit *unit = calloc(1, sizeof(*unit));
    unit->type = SUT_FOR;

    unit->tokens = tokens;
    unit->for_stmt.init = init;
    unit->for_stmt.cond = cond;
    unit->for_stmt.step = step;
    unit->for_stmt.body = body;

    return unit;
}

void statement_unit_while_free(StatementUnit *unit)
{
    if (!unit || unit->type != SUT_WHILE)
        return;
    statement_unit_free_tokens(unit->tokens);
    statement_unit_free(unit->while_stmt.cond);
    statement_unit_free(unit->while_stmt.body);
    free(unit);
}

void statement_unit_do_while_free(StatementUnit *unit)
{
    if (!unit || unit->type != SUT_DO_WHILE)
        return;
    statement_unit_free_tokens(unit->tokens);
    statement_unit_free(unit->do_while_stmt.body);
    statement_unit_free(unit->do_while_stmt.cond);
    free(unit);
}

void statement_unit_for_free(StatementUnit *unit)
{
    if (!unit || unit->type != SUT_FOR)
        return;
    statement_unit_free_tokens(unit->tokens);
    statement_unit_free(unit->for_stmt.init);
    statement_unit_free(unit->for_stmt.cond);
    statement_unit_free(unit->for_stmt.step);
    statement_unit_free(unit->for_stmt.body);
    free(unit);
}