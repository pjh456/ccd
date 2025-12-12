#pragma once

typedef struct StatementUnit StatementUnit;

void statement_unit_while_free(StatementUnit *unit);
void statement_unit_do_while_free(StatementUnit *unit);
void statement_unit_for_free(StatementUnit *unit);
