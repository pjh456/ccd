#include "parser_impl/expression.h"
#include "parser_impl/expression_simple_impl.h"
#include "parser_impl/c_type_info.h"
#include <stdlib.h>

Expression *make_expression_subscript(Expression *base, Expression *index)
{
    if (!base || !index)
        return NULL;
    Expression *val = malloc(sizeof(*val));
    val->type_info = c_type_info_copy(base->type_info->array.base);
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
    val->type_info = NULL; // 需要查成员表
    val->type = EXPR_MEMBER;

    val->member.base = base;
    val->member.mem = mem;
    return val;
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
    expression_free(expr->member.base);
    expression_free(expr->member.mem);
    free(expr);
}

void expression_ptr_member_free(Expression *expr)
{
    if (!expr || expr->type != EXPR_PTR_MEMBER)
        return;
    expression_free(expr->member.base);
    expression_free(expr->member.mem);
    free(expr);
}