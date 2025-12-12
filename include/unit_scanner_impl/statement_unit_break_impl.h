#pragma once

typedef struct StatementUnit StatementUnit;

void statement_unit_continue_free(StatementUnit *unit);
void statement_unit_break_free(StatementUnit *unit);
void statement_unit_return_free(StatementUnit *unit);