#pragma once

typedef struct StatementUnit StatementUnit;

void statement_unit_label_free(StatementUnit *unit);
void statement_unit_goto_free(StatementUnit *unit);