#pragma once

typedef struct Vector Vector;
typedef struct StatementUnit StatementUnit;

void statement_unit_free_tokens(Vector *);

void print_statement_unit_impl(StatementUnit *unit, int indent, int token_printed);

void print_tokens(Vector *tokens);