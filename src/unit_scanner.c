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

Token *peek_token(UnitScanner *us) { return (Token *)vector_get(us->tokens, us->pos); }
Token *next_token(UnitScanner *us) { return (Token *)vector_get(us->tokens, us->pos++); }

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
        return scan_decl_or_expression(us);
    }
}

StatementUnit *scan_identifier(UnitScanner *us)
{
    if (!us)
        return NULL;
    if (peek_token(us)->type != T_IDENTIFIER)
        return NULL;

    Token *t = (Token *)vector_get(us->tokens, us->pos);
    if (!t)
        return NULL;

    switch (t->type)
    {
    case T_COLON:
        return scan_label(us);
    case T_LEFT_PAREN:
    case T_COMMA:

    case T_AND:         // &
    case T_OR:          // |
    case T_XOR:         // ^
    case T_LEFT_SHIFT:  // <<
    case T_RIGHT_SHIFT: // >>

    case T_PLUS:   // +
    case T_MINUS:  // -
    case T_STAR:   // *
    case T_DIV:    // /
    case T_MOD:    // %
    case T_ASSIGN: // =

    case T_LESS:          // <
    case T_GREATER:       // >
    case T_EQUAL:         // ==
    case T_NOT_EQUAL:     // !=
    case T_LESS_EQUAL:    // <=
    case T_GREATER_EQUAL: // >=
    case T_AND_AND:       // &&
    case T_OR_OR:         // ||

    case T_AND_ASSIGN:         // &=
    case T_OR_ASSIGN:          // |=
    case T_XOR_ASSIGN:         // ^=
    case T_LEFT_SHIFT_ASSIGN:  // <<=
    case T_RIGHT_SHIFT_ASSIGN: // <<=

    case T_PLUS_ASSIGN:  // +=
    case T_MINUS_ASSIGN: // -=
    case T_MUL_ASSIGN:   // *=
    case T_DIV_ASSIGN:   // /=
    case T_MOD_ASSIGN:   // %=

    case T_INC: // ++
    case T_DEC: // --
        return scan_decl_or_expression(us);
    default:
        return NULL;
    }
}