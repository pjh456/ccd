#include "unit_scanner_impl/statement_unit.h"
#include "unit_scanner_impl/statement_unit_impl/statement_unit_impl.h"
#include "unit_scanner_impl/statement_unit_impl/statement_unit_label_impl.h"
#include "vector.h"
#include "utils.h"
#include <stdlib.h>

StatementUnit *make_label_statement_unit(Vector *tokens, char *name)
{
    if (!tokens || !name)
        return NULL;
    StatementUnit *unit = calloc(1, sizeof(*unit));
    unit->type = SUT_LABEL;

    unit->tokens = tokens;
    unit->label_stmt.name = str_clone(name);

    return unit;
}

StatementUnit *make_goto_statement_unit(Vector *tokens, char *name)
{
    if (!tokens || !name)
        return NULL;
    StatementUnit *unit = calloc(1, sizeof(*unit));
    unit->type = SUT_GOTO;

    unit->tokens = tokens;
    unit->goto_stmt.name = str_clone(name);

    return unit;
}

void statement_unit_label_free(StatementUnit *unit)
{
    if (!unit || unit->type != SUT_LABEL)
        return;
    statement_unit_free_tokens(unit->tokens);
    if (unit->label_stmt.name)
        free(unit->label_stmt.name);
    free(unit);
}

void statement_unit_goto_free(StatementUnit *unit)
{
    if (!unit || unit->type != SUT_GOTO)
        return;
    statement_unit_free_tokens(unit->tokens);
    if (unit->goto_stmt.name)
        free(unit->goto_stmt.name);
    free(unit);
}