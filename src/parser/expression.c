#include "parser_impl/expression.h"
#include "parser_impl/expression_literal_impl.h"
#include "parser_impl/expression_identifier_impl.h"
#include <stdlib.h>

void expression_free(Expression *expr)
{
    if (!expr)
        return;
    switch (expr->type)
    {
    case EXPR_LITERAL:
        expression_literal_free(expr);
        break;
    case EXPR_IDENTIFIER:
        expression_identifier_free(expr);
        break;
    case EXPR_CAST:
        expression_cast_free(expr);
        break;
    case EXPR_ASSIGN:
        break;
    case EXPR_UNARY:
        break;
    case EXPR_BINARY:
        break;

    case EXPR_CALL:
        break;
    case EXPR_SIZEOF:
        break;

    case EXPR_SUBSCRIPT:
    case EXPR_MEMBER:
    case EXPR_PTR_MEMBER:

    case EXPR_CONDITIONAL:
    case EXPR_COMMA:
    case EXPR_PAREN:

    case EXPR_UNKNOWN:
    default:
        free(expr);
    }
}