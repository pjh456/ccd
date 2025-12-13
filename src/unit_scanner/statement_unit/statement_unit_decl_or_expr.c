#include "unit_scanner_impl/statement_unit.h"
#include "unit_scanner_impl/statement_unit_impl/statement_unit_impl.h"
#include "unit_scanner_impl/statement_unit_impl/statement_unit_decl_or_expr_impl.h"
#include "vector.h"
#include <stdlib.h>

StatementUnit *make_decl_or_expr_statement_unit(Vector *tokens)
{
    if (!tokens)
        return NULL;
    StatementUnit *unit = calloc(1, sizeof(*unit));
    unit->type = SUT_DECL_OR_EXPR;

    unit->tokens = tokens;

    return unit;
}

void statement_unit_decl_or_expr_free(StatementUnit *unit)
{
    if (!unit || unit->type != SUT_DECL_OR_EXPR)
        return;
    statement_unit_free_tokens(unit->tokens);
    free(unit);
}