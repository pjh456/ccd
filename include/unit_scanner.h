#pragma once

#include <stddef.h>

typedef struct Vector Vector;
typedef struct Token Token;
typedef struct StatementUnit StatementUnit;
typedef struct UnitScanner UnitScanner;

struct UnitScanner
{
    Vector *tokens;
    size_t pos;
};

UnitScanner *unit_scanner_new(Vector *tokens);

void unit_scanner_free(UnitScanner *us);

Token *peek_token(UnitScanner *us);
Token *next_token(UnitScanner *us);

StatementUnit *scan_unit(UnitScanner *us);
StatementUnit *scan_identifier(UnitScanner *us);