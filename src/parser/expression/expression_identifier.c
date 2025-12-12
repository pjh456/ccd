#include "parser_impl/expression.h"
#include "parser_impl/expression_impl/expression_identifier_impl.h"
#include "parser_impl/c_type_info.h"
#include <stdlib.h>
#include <string.h>

Expression *make_expression_identifier(char *name)
{
    if (!name)
        return NULL;
    Expression *expr = malloc(sizeof(*expr));
    memset(expr, 0, sizeof(*expr));

    expr->type_info = NULL; // 等查变量表的时候才有类型
    expr->type = EXPR_IDENTIFIER;

    expr->ident.name = malloc(strlen(name) + 1);
    strcpy(expr->ident.name, name);
    return expr;
}

Expression *make_expression_cast(CTypeInfo *cti, Expression *expr)
{
    if (!expr || !cti)
        return NULL;
    Expression *cast = malloc(sizeof(*cast));
    memset(cast, 0, sizeof(*cast));

    cast->type_info = cti;
    cast->type = EXPR_CAST;

    cast->cast.type_info = cti;
    cast->cast.expr = expr;
    return cast;
}

Expression *make_expression_assign(AssignOperator op, Expression *lhs, Expression *rhs)
{
    if (!lhs || !rhs)
        return NULL;
    Expression *assign = malloc(sizeof(*assign));
    memset(assign, 0, sizeof(*assign));

    assign->type_info = c_type_info_copy(lhs->type_info);
    assign->type = EXPR_ASSIGN;

    assign->assign.op = op;
    assign->assign.lhs = lhs;
    assign->assign.rhs = rhs;
    return assign;
}

void expression_identifier_free(Expression *expr)
{
    if (!expr || expr->type != EXPR_IDENTIFIER)
        return;
    c_type_info_free(expr->type_info);
    if (expr->ident.name)
        free(expr->ident.name);
    free(expr);
}

void expression_cast_free(Expression *expr)
{
    if (!expr || expr->type != EXPR_CAST)
        return;
    c_type_info_free(expr->type_info);
    c_type_info_free(expr->cast.type_info);
    expression_free(expr->cast.expr);
    free(expr);
}

void expression_assign_free(Expression *expr)
{
    if (!expr || expr->type != EXPR_ASSIGN)
        return;
    c_type_info_free(expr->type_info);
    expression_free(expr->assign.lhs);
    expression_free(expr->assign.rhs);
    free(expr);
}