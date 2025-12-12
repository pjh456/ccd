#include "unit_scanner_impl/statement_unit.h"
#include "unit_scanner_impl/statement_unit_impl.h"
#include "unit_scanner_impl/statement_unit_compound_impl.h"
#include "unit_scanner_impl/statement_unit_declaration_impl.h"
#include "unit_scanner_impl/statement_unit_conditional_impl.h"
#include "unit_scanner_impl/statement_unit_loop_impl.h"
#include "unit_scanner_impl/statement_unit_break_impl.h"
#include "unit_scanner_impl/statement_unit_label_impl.h"
#include "vector.h"
#include <stdlib.h>

void statement_unit_free(StatementUnit *unit)
{
    if (!unit)
        return;

    switch (unit->type)
    {
    case SUT_COMPOUND:
        statement_unit_compound_free(unit);
        break;
    case SUT_EMPTY:
        statement_unit_empty_free(unit);
        break;
    case SUT_DECL:
        statement_unit_declaration_free(unit);
        break;
    case SUT_EXPR:
        statement_unit_expression_free(unit);
        break;
    case SUT_IF:
        statement_unit_if_free(unit);
        break;
    case SUT_SWITCH:
        statement_unit_switch_free(unit);
        break;
    case SUT_CASE:
        statement_unit_case_free(unit);
        break;
    case SUT_DEFAULT:
        statement_unit_default_free(unit);
        break;
    case SUT_WHILE:
        statement_unit_while_free(unit);
        break;
    case SUT_DO_WHILE:
        statement_unit_do_while_free(unit);
        break;
    case SUT_FOR:
        statement_unit_for_free(unit);
        break;
    case SUT_CONTINUE:
        statement_unit_continue_free(unit);
        break;
    case SUT_BREAK:
        statement_unit_break_free(unit);
        break;
    case SUT_RETURN:
        statement_unit_return_free(unit);
        break;
    case SUT_LABEL:
        statement_unit_label_free(unit);
        break;
    case SUT_GOTO:
        statement_unit_goto_free(unit);
        break;
    default:
        free(unit);
    }
}

void statement_unit_free_tokens(Vector *tokens)
{
    if (!tokens)
        return;
    vector_free(tokens);
}