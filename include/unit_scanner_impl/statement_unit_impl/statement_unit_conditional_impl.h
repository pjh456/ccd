#pragma once

typedef struct StatementUnit StatementUnit;

void statement_unit_if_free(StatementUnit *unit);
void statement_unit_switch_free(StatementUnit *unit);
void statement_unit_case_free(StatementUnit *unit);
void statement_unit_default_free(StatementUnit *unit);