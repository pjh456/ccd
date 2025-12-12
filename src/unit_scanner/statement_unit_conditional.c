#include "unit_scanner_impl/statement_unit.h"
#include "unit_scanner_impl/statement_unit_impl.h"
#include "unit_scanner_impl/statement_unit_conditional_impl.h"
#include "vector.h"
#include <stdlib.h>

StatementUnit *make_if_statement_unit(
    Vector *tokens,
    StatementUnit *cond,
    StatementUnit *then_body,
    StatementUnit *else_body)
{
    if (!tokens || !cond || !then_body)
        return NULL;
    StatementUnit *unit = calloc(1, sizeof(*unit));
    unit->type = SUT_IF;

    unit->tokens = tokens;
    unit->if_stmt.cond = cond;
    unit->if_stmt.then_body = then_body;
    unit->if_stmt.else_body = else_body;

    return unit;
}

StatementUnit *make_switch_statement_unit(
    Vector *tokens,
    StatementUnit *expr,
    StatementUnit *body)
{
    if (!tokens || !expr || !body)
        return NULL;
    StatementUnit *unit = calloc(1, sizeof(*unit));
    unit->type = SUT_SWITCH;

    unit->tokens = tokens;
    unit->switch_stmt.expr = expr;
    unit->switch_stmt.body = body;

    return unit;
}

StatementUnit *make_case_statement_unit(Vector *tokens, StatementUnit *expr)
{
    if (!tokens || !expr)
        return NULL;
    StatementUnit *unit = calloc(1, sizeof(*unit));
    unit->type = SUT_CASE;

    unit->tokens = tokens;
    unit->case_stmt.expr = expr;

    return unit;
}

StatementUnit *make_default_statement_unit(Vector *tokens, int dummy)
{
    if (!tokens)
        return NULL;
    StatementUnit *unit = calloc(1, sizeof(*unit));
    unit->type = SUT_DEFAULT;

    unit->tokens = tokens;
    unit->default_stmt.dummy = dummy;

    return unit;
}

void statement_unit_if_free(StatementUnit *unit)
{
    if (!unit || unit->type != SUT_IF)
        return;
    statement_unit_free_tokens(unit->tokens);
    statement_unit_free(unit->if_stmt.cond);
    statement_unit_free(unit->if_stmt.then_body);
    statement_unit_free(unit->if_stmt.else_body);
    free(unit);
}

void statement_unit_switch_free(StatementUnit *unit)
{
    if (!unit || unit->type != SUT_SWITCH)
        return;
    statement_unit_free_tokens(unit->tokens);
    statement_unit_free(unit->switch_stmt.expr);
    statement_unit_free(unit->switch_stmt.body);
    free(unit);
}

void statement_unit_case_free(StatementUnit *unit)
{
    if (!unit || unit->type != SUT_DEFAULT)
        return;
    statement_unit_free_tokens(unit->tokens);
    statement_unit_free(unit->case_stmt.expr);
    free(unit);
}

void statement_unit_default_free(StatementUnit *unit)
{
    if (!unit || unit->type != SUT_DEFAULT)
        return;
    statement_unit_free_tokens(unit->tokens);
    free(unit);
}