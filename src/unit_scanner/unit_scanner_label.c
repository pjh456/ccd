#include "unit_scanner.h"
#include "unit_scanner_impl/unit_scanner_impl.h"
#include "unit_scanner_impl/statement_unit.h"
#include "tokenizer_impl/token.h"
#include "vector.h"
#include "utils.h"

StatementUnit *scan_label(UnitScanner *us)
{
    if (!us)
        return NULL;
    if (peek_token(us)->type != T_IDENTIFIER)
        return NULL;

    size_t pos = us->pos;
    Token *t = next_token(us); // identifier

    if (peek_token(us)->type != T_COLON)
        return NULL;
    next_token(us);

    StatementUnit *unit = make_label_statement_unit(
        vector_slice(us->tokens, pos, us->pos),
        str_clone(t->str));

    return unit;
}

StatementUnit *scan_goto(UnitScanner *us)
{
    if (!us)
        return NULL;
    if (peek_token(us)->type != T_GOTO)
        return NULL;

    size_t pos = us->pos;
    next_token(us); // goto

    if (peek_token(us)->type != T_IDENTIFIER)
        return NULL;
    Token *t = next_token(us); // identifier

    if (peek_token(us)->type != T_SEMICOLON)
        return NULL;
    next_token(us);

    StatementUnit *unit = make_goto_statement_unit(
        vector_slice(us->tokens, pos, us->pos),
        str_clone(t->str));

    return unit;
}