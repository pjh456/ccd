#include "unit_scanner.h"
#include "unit_scanner_impl/unit_scanner_impl.h"
#include "unit_scanner_impl/statement_unit.h"
#include "tokenizer_impl/token.h"
#include "vector.h"

StatementUnit *scan_empty(UnitScanner *us)
{
    if (!us)
        return NULL;
    if (peek_token(us)->type != T_SEMICOLON)
        return NULL;

    StatementUnit *unit = make_empty_statement_unit(
        vector_slice(us->tokens, us->pos, us->pos + 1));

    next_token(us); // ;
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
        vector_push_back(units, scan_unit(us));

        if (peek_token(us)->type != T_SEMICOLON) // error
            break;
        next_token(us); // ;

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