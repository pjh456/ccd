#pragma once

typedef struct StatementUnit StatementUnit;

void statement_unit_declaration_free(StatementUnit *unit);
void statement_unit_expression_free(StatementUnit *unit);