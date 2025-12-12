#include "parser_impl/expression.h"
#include "parser_impl/expression_impl/expression_simple_impl.h"
#include "parser_impl/c_type_info.h"
#include "vector.h"
#include <stdlib.h>
#include <string.h>

Expression *make_expression_subscript(Expression *base, Expression *index)
{
    if (!base || !index)
        return NULL;
    Expression *val = malloc(sizeof(*val));
    memset(val, 0, sizeof(*val));

    if (base->type_info)
        val->type_info = c_type_info_copy(base->type_info->array.base);
    else
        val->type_info = NULL;
    val->type = EXPR_SUBSCRIPT;

    val->subscript.base = base;
    val->subscript.index = index;
    return val;
}

Expression *make_expression_member(Expression *base, Expression *mem)
{
    if (!base || !mem)
        return NULL;
    Expression *val = malloc(sizeof(*val));
    memset(val, 0, sizeof(*val));

    val->type_info = NULL; // 需要查成员表
    val->type = EXPR_MEMBER;

    val->member.base = base;
    val->member.mem = mem;
    return val;
}

Expression *make_expression_ptr_member(Expression *base, Expression *mem)
{
    if (!base || !mem)
        return NULL;
    Expression *val = malloc(sizeof(*val));
    memset(val, 0, sizeof(*val));

    val->type_info = NULL; // 需要查成员表
    val->type = EXPR_MEMBER;

    val->member.base = base;
    val->member.mem = mem;
    return val;
}

Expression *make_expression_conditional(Expression *cond, Expression *then_expr, Expression *else_expr)
{
    if (!cond || !then_expr || !else_expr)
        return NULL;
    Expression *conditional = malloc(sizeof(*conditional));
    memset(conditional, 0, sizeof(*conditional));

    conditional->type_info = c_type_info_copy(then_expr->type_info);
    conditional->type = EXPR_CONDITIONAL;

    conditional->conditional.cond = cond;
    conditional->conditional.then_expr = then_expr;
    conditional->conditional.else_expr = else_expr;
    return conditional;
}

Expression *make_expression_comma(Vector *exprs)
{
    if (!exprs || exprs->size == 0)
        return NULL;
    Expression *comma = malloc(sizeof(*comma));
    memset(comma, 0, sizeof(*comma));

    comma->type_info = c_type_info_copy((*((Expression **)vector_back(exprs)))->type_info);
    comma->type = EXPR_COMMA;

    comma->comma.exprs = exprs;
    return comma;
}

Expression *make_expression_paren(Expression *expr)
{
    if (!expr)
        return NULL;
    Expression *paren = malloc(sizeof(*paren));
    memset(paren, 0, sizeof(*paren));

    paren->type_info = c_type_info_copy(expr->type_info);
    paren->type = EXPR_PAREN;

    paren->paren.expr = expr;
    return paren;
}

void expression_subscript_free(Expression *expr)
{
    if (!expr || expr->type != EXPR_SUBSCRIPT)
        return;
    c_type_info_free(expr->type_info);
    expression_free(expr->subscript.base);
    expression_free(expr->subscript.index);
    free(expr);
}

void expression_member_free(Expression *expr)
{
    if (!expr || expr->type != EXPR_MEMBER)
        return;
    c_type_info_free(expr->type_info);
    expression_free(expr->member.base);
    expression_free(expr->member.mem);
    free(expr);
}

void expression_ptr_member_free(Expression *expr)
{
    if (!expr || expr->type != EXPR_PTR_MEMBER)
        return;
    c_type_info_free(expr->type_info);
    expression_free(expr->member.base);
    expression_free(expr->member.mem);
    free(expr);
}

void expression_conditional_free(Expression *expr)
{
    if (!expr || expr->type != EXPR_CONDITIONAL)
        return;
    c_type_info_free(expr->type_info);
    expression_free(expr->conditional.cond);
    expression_free(expr->conditional.then_expr);
    expression_free(expr->conditional.else_expr);
    free(expr);
}

void expression_comma_free(Expression *expr)
{
    if (!expr || expr->type != EXPR_COMMA)
        return;
    c_type_info_free(expr->type_info);
    if (expr->comma.exprs)
    {
        for (size_t idx = 0; idx < expr->comma.exprs->size; ++idx)
            expression_free(*((Expression **)vector_get(expr->comma.exprs, idx)));
        vector_free(expr->comma.exprs);
    }
    free(expr);
}

void expression_paren_free(Expression *expr)
{
    if (!expr || expr->type != EXPR_PAREN)
        return;
    c_type_info_free(expr->type_info);
    expression_free(expr->paren.expr);
    free(expr);
}