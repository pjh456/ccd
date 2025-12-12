#include "unit_scanner.h"
#include "unit_scanner_impl/unit_scanner_impl.h"
#include "unit_scanner_impl/statement_unit.h"
#include "tokenizer.h"
#include "tokenizer_impl/token.h"
#include "vector.h"
#include <malloc.h>

UnitScanner *unit_scanner_new(Vector *tokens)
{
    UnitScanner *us = malloc(sizeof(*us));
    us->tokens = tokens;
    us->pos = 0;
    return us;
}

void unit_scanner_free(UnitScanner *us)
{
    if (!us)
        return;
    vector_free(us->tokens);
    free(us);
}

Token *peek_token(UnitScanner *us) { return vector_get(us->tokens, us->pos); }
Token *next_token(UnitScanner *us) { return vector_get(us->tokens, us->pos++); }

StatementUnit *scan_unit(UnitScanner *us)
{
    if (!us)
        return NULL;

    Token *t = peek_token(us);
    switch (t->type)
    {
    case T_IDENTIFIER:
        return scan_identifier(us);
    case T_SEMICOLON:
        return scan_empty(us);
    case T_LEFT_BRACE:
        return scan_compound(us);
    case T_EXTERN:
    case T_STATIC:
    case T_INLINE:
    case T_REGISTER:
    case T_RESTRICT:
    case T_VOLATILE:
    case T_CONST:
    case T_AUTO:
    case T_VOID:
    case T_SIGNED:
    case T_UNSIGNED:
    case T_CHAR:
    case T_SHORT:
    case T_INT:
    case T_LONG:
    case T_FLOAT:
    case T_DOUBLE:
        return scan_declaration(us);
    case T_LEFT_PAREN:
        return scan_expression(us);
    case T_IF:
        return scan_if(us);
    case T_SWITCH:
        return scan_switch(us);
    case T_CASE:
        return scan_case(us);
    case T_DEFAULT:
        return scan_default(us);
    case T_WHILE:
        return scan_while(us);
    case T_DO:
        return scan_do_while(us);
    case T_FOR:
        return scan_for(us);
    case T_CONTINUE:
        return scan_continue(us);
    case T_BREAK:
        return scan_break(us);
    case T_RETURN:
        return scan_return(us);
    case T_GOTO:
        return scan_goto(us);
    default:
        return NULL;
    }
}

StatementUnit *scan_identifier(UnitScanner *us)
{
}