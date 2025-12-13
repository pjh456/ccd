#include "unit_scanner.h"
#include "unit_scanner_impl/unit_scanner_impl.h"
#include "unit_scanner_impl/statement_unit.h"
#include "tokenizer_impl/token.h"
#include "vector.h"

StatementUnit *scan_if(UnitScanner *us)
{
    if (!us)
        return NULL;
    if (peek_token(us)->type != T_IF)
        return NULL;

    size_t pos = us->pos;
    next_token(us); // if

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

    StatementUnit *then_body = scan_unit(us);
    StatementUnit *else_body = NULL;
    if (peek_token(us)->type == T_ELSE)
    {
        next_token(us); // else
        else_body = scan_unit(us);
    }

    StatementUnit *unit = make_if_statement_unit(
        vector_slice(us->tokens, pos, us->pos),
        cond, then_body, else_body);

    return unit;
}

StatementUnit *scan_switch(UnitScanner *us)
{
    if (!us)
        return NULL;
    if (peek_token(us)->type != T_SWITCH)
        return NULL;

    size_t pos = us->pos;
    next_token(us); // switch

    if (peek_token(us)->type != T_LEFT_PAREN)
        return NULL;
    next_token(us); // (

    StatementUnit *expr = scan_decl_or_expression(us, 1);

    if (peek_token(us)->type != T_RIGHT_PAREN)
    {
        statement_unit_free(expr);
        return NULL;
    }
    next_token(us); // )

    StatementUnit *body = scan_compound(us);

    StatementUnit *unit = make_switch_statement_unit(
        vector_slice(us->tokens, pos, us->pos),
        expr, body);

    return unit;
}

StatementUnit *scan_case(UnitScanner *us)
{
    if (!us)
        return NULL;
    if (peek_token(us)->type != T_CASE)
        return NULL;

    size_t pos = us->pos;
    next_token(us); // case

    StatementUnit *expr = scan_decl_or_expression(us, 1);

    if (peek_token(us)->type != T_COLON)
    {
        statement_unit_free(expr);
        return NULL;
    }
    next_token(us); // :

    StatementUnit *unit = make_case_statement_unit(
        vector_slice(us->tokens, pos, us->pos),
        expr);

    return unit;
}

StatementUnit *scan_default(UnitScanner *us)
{
    if (!us)
        return NULL;
    if (peek_token(us)->type != T_DEFAULT)
        return NULL;

    size_t pos = us->pos;
    next_token(us); // default

    if (peek_token(us)->type != T_COLON)
        return NULL;
    next_token(us); // :

    StatementUnit *unit = make_default_statement_unit(
        vector_slice(us->tokens, pos, us->pos), 0);

    return unit;
}