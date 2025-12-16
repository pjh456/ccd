#pragma once

#include <stddef.h>

typedef struct Vector Vector;
typedef struct StatementUnit StatementUnit;
typedef struct DeclUnit DeclUnit;
typedef struct DeclParser DeclParser;

struct DeclParser
{
    Vector *stmts; // StatementUnits
    size_t stmt_pos;
    size_t token_pos;
};

DeclParser *decl_parser_new(Vector *stmts);

void decl_parser_free(DeclParser *dp);

StatementUnit *peek_statement(DeclParser *dp);
StatementUnit *next_statement(DeclParser *dp);

Vector *parse_file_decl(DeclParser *dp); // DeclUnits