#include "unit_scanner.h"
#include "unit_scanner_impl/unit_scanner_impl.h"
#include "unit_scanner_impl/statement_unit.h"
#include "tokenizer_impl/token.h"
#include "vector.h"

StatementUnit *scan_while(UnitScanner *us)
{
    if (!us)
        return NULL;
    if (peek_token(us)->type != T_WHILE)
        return NULL;

    size_t pos = us->pos;
    next_token(us); // while

    if (peek_token(us)->type != T_LEFT_PAREN)
        return NULL;
    next_token(us); // (

    StatementUnit *cond = scan_decl_or_expression(us, 1);
    if (peek_token(us)->type != T_RIGHT_PAREN)
    {
        statement_unit_free(cond);
        return NULL;
    }
    next_token(us); // )

    StatementUnit *body = scan_unit(us);

    StatementUnit *unit = make_while_statement_unit(
        vector_slice(us->tokens, pos, us->pos), cond, body);

    return unit;
}

StatementUnit *scan_do_while(UnitScanner *us)
{
    if (!us)
        return NULL;
    if (peek_token(us)->type != T_DO)
        return NULL;

    size_t pos = us->pos;
    next_token(us); // do

    StatementUnit *body = scan_unit(us);

    if (peek_token(us)->type != T_WHILE)
    {
        statement_unit_free(body);
        return NULL;
    }
    next_token(us); // while

    if (peek_token(us)->type != T_LEFT_PAREN)
        return NULL;
    next_token(us); // (

    StatementUnit *cond = scan_decl_or_expression(us, 1);
    if (peek_token(us)->type != T_RIGHT_PAREN)
    {
        statement_unit_free(cond);
        return NULL;
    }
    next_token(us); // )

    if (peek_token(us)->type != T_SEMICOLON)
    {
        statement_unit_free(body);
        statement_unit_free(cond);
        return NULL;
    }
    next_token(us); // ;

    StatementUnit *unit = make_do_while_statement_unit(
        vector_slice(us->tokens, pos, us->pos),
        body, cond);

    return unit;
}

StatementUnit *scan_for(UnitScanner *us)
{
    if (!us)
        return NULL;
    if (peek_token(us)->type != T_FOR)
        return NULL;

    size_t pos = us->pos;
    next_token(us); // for

    if (peek_token(us)->type != T_LEFT_PAREN)
        return NULL;
    next_token(us); // (

    StatementUnit *init = scan_decl_or_expression(us, 0);
    StatementUnit *cond = scan_decl_or_expression(us, 0);
    StatementUnit *step = scan_decl_or_expression(us, 1);

    if (peek_token(us)->type != T_RIGHT_PAREN)
    {
        statement_unit_free(init);
        statement_unit_free(cond);
        statement_unit_free(step);
        return NULL;
    }
    next_token(us); // )

    StatementUnit *body = scan_unit(us);

    StatementUnit *unit = make_for_statement_unit(
        vector_slice(us->tokens, pos, us->pos),
        init, cond, step, body);

    return unit;
}