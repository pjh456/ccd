#pragma once

typedef struct StatementUnit StatementUnit;

void statement_unit_compound_free(StatementUnit *unit);
void statement_unit_empty_free(StatementUnit *unit);