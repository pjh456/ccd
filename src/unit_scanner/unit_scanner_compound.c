#include "unit_scanner.h"
#include "unit_scanner_impl/unit_scanner_impl.h"
#include "unit_scanner_impl/statement_unit.h"
#include "tokenizer_impl/token.h"
#include "vector.h"
#include <stdlib.h>

StatementUnit *scan_empty(UnitScanner *us)
{
    if (!us)
        return NULL;
    if (peek_token(us)->type != T_SEMICOLON)
        return NULL;

    size_t pos = us->pos;
    next_token(us); // ;

    StatementUnit *unit = make_empty_statement_unit(
        vector_slice(us->tokens, pos, us->pos));

    return unit;
}

StatementUnit *scan_compound(UnitScanner *us)
{
    if (!us)
        return NULL;
    if (peek_token(us)->type != T_LEFT_BRACE)
        return NULL;

    size_t pos = us->pos;
    next_token(us); // {

    Vector *units = vector_new(sizeof(StatementUnit *));
    while (peek_token(us)->type != T_EOF)
    {
        StatementUnit *ptr = scan_unit(us);
        vector_push_back(units, &ptr);

        if (peek_token(us)->type == T_RIGHT_BRACE)
        {
            next_token(us); // }
            break;
        }
    }

    StatementUnit *unit = make_compound_statement_unit(
        vector_slice(us->tokens, pos, us->pos),
        units);

    return unit;
}

StatementUnit *scan_preprocessor(UnitScanner *us)
{
    if (!us)
        return NULL;
    if (peek_token(us)->type != T_PREPROCESS)
        return NULL;

    size_t pos = us->pos;
    next_token(us); // #...

    StatementUnit *unit = make_preprocessor_statement_unit(
        vector_slice(us->tokens, pos, us->pos));

    return unit;
}