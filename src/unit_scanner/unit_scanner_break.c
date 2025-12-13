#include "unit_scanner.h"
#include "unit_scanner_impl/unit_scanner_impl.h"
#include "unit_scanner_impl/statement_unit.h"
#include "tokenizer_impl/token.h"
#include "vector.h"

StatementUnit *scan_continue(UnitScanner *us)
{
    if (!us)
        return NULL;
    if (peek_token(us)->type != T_CONTINUE)
        return NULL;

    size_t pos = us->pos;
    next_token(us);

    if (peek_token(us)->type != T_SEMICOLON)
        return NULL;
    next_token(us); // ;

    StatementUnit *unit = make_continue_statement_unit(
        vector_slice(us->tokens, pos, us->pos));

    return unit;
}

StatementUnit *scan_break(UnitScanner *us)
{
    if (!us)
        return NULL;
    if (peek_token(us)->type != T_BREAK)
        return NULL;

    size_t pos = us->pos;
    next_token(us);

    if (peek_token(us)->type != T_SEMICOLON)
        return NULL;
    next_token(us); // ;

    StatementUnit *unit = make_break_statement_unit(
        vector_slice(us->tokens, pos, us->pos));

    return unit;
}

StatementUnit *scan_return(UnitScanner *us)
{
    if (!us)
        return NULL;
    if (peek_token(us)->type != T_RETURN)
        return NULL;

    size_t pos = us->pos;
    next_token(us); // return

    StatementUnit *expr = scan_decl_or_expression(us, 1);

    if (peek_token(us)->type != T_SEMICOLON)
    {
        statement_unit_free(expr);
        return NULL;
    }
    next_token(us); // ;

    StatementUnit *unit = make_return_statement_unit(
        vector_slice(us->tokens, pos, us->pos), expr);

    return unit;
}