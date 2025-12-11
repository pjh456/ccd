#include "parser_impl/expression.h"
#include "parser_impl/c_type_info.h"
#include "parser_impl/expression_operator_impl.h"
#include <stdlib.h>
#include <string.h>

Expression *make_expression_unary(UnaryOperator op, Expression *expr, int is_prefix)
{
    if (!expr)
        return NULL;
    Expression *unary = malloc(sizeof(*unary));
    memset(unary, 0, sizeof(*unary));

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

Expression *make_expression_binary(BinaryOperator op, Expression *lhs, Expression *rhs)
{
    if (!lhs || !rhs)
        return NULL;
    Expression *binary = malloc(sizeof(*binary));
    memset(binary, 0, sizeof(*binary));

    binary->type_info = c_type_info_copy(lhs->type_info);
    binary->type = EXPR_BINARY;

    binary->binary.op = op;
    binary->binary.lhs = lhs;
    binary->binary.rhs = rhs;
    return binary;
}

void expression_unary_free(Expression *expr)
{
    if (!expr || expr->type != EXPR_UNARY)
        return;
    c_type_info_free(expr->type_info);
    expression_free(expr->unary.expr);
    free(expr);
}

void expression_binary_free(Expression *expr)
{
    if (!expr || expr->type != EXPR_BINARY)
        return;
    c_type_info_free(expr->type_info);
    expression_free(expr->binary.lhs);
    expression_free(expr->binary.rhs);
    free(expr);
}