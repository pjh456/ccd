#include "unit_scanner_impl/statement_unit.h"
#include "unit_scanner_impl/statement_unit_impl.h"
#include "vector.h"
#include <stdlib.h>

void statement_unit_free(StatementUnit *unit)
{
    if (!unit)
        return;

    switch (unit->type)
    {
    case SUT_COMPOUND:
        break;
    case SUT_EMPTY:
        break;
    case SUT_DECL:
        break;
    case SUT_EXPR:
        break;
    case SUT_IF:
        break;
    case SUT_SWITCH:
        break;
    case SUT_CASE:
        break;
    case SUT_DEFAULT:
        break;
    case SUT_WHILE:
        break;
    case SUT_DO_WHILE:
        break;
    case SUT_FOR:
        break;
    case SUT_CONTINUE:
        break;
    case SUT_BREAK:
        break;
    case SUT_RETURN:
        break;
    case SUT_LABEL:
        break;
    case SUT_GOTO:
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