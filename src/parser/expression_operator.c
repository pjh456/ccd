#include "parser_impl/expression.h"
#include "parser_impl/c_type_info.h"
#include <stdlib.h>

Expression *make_expression_unary(UnaryOperator op, Expression *expr, int is_prefix)
{
    if (!expr)
        return NULL;
    Expression *unary = malloc(sizeof(*unary));
    if (op == OP_NOT)
        unary->type_info = make_char_type(CTS_NONE, CTQ_NONE, CTM_SIGNED);
    else
        unary->type_info = c_type_info_copy(expr->type_info);
    unary->type = EXPR_UNARY;

    unary->unary.op = op;
    unary->unary.expr = expr;
    unary->unary.is_prefix = is_prefix;
    return unary;
}