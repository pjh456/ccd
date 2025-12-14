#pragma once

typedef struct DeclUnit DeclUnit;

void decl_unit_declaration_free(DeclUnit *unit);
void decl_unit_expression_free(DeclUnit *unit);
void decl_unit_statement_free(DeclUnit *unit);

void print_decl_unit_impl(DeclUnit *unit, int indent);