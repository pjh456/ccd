#include "parser_impl/expression.h"
#include "parser_impl/expression_function_impl.h"
#include "parser_impl/c_type_info.h"
#include "vector.h"
#include <stdlib.h>
#include <string.h>

Expression *make_expression_call(Expression *func, Vector *args)
{
    if (!func)
        return NULL;
    Expression *call = malloc(sizeof(*call));
    memset(call, 0, sizeof(*call));

    call->type_info = NULL; // 查函数表时再补全
    call->type = EXPR_CALL;

    call->call.func = func;
    call->call.args = args;
    return call;
}

Expression *make_expression_sizeof(Expression *expr)
{
    if (!expr)
        return NULL;
    Expression *call = malloc(sizeof(*call));
    memset(call, 0, sizeof(*call));

    call->type_info = make_int_type(CTS_NONE, CTQ_NONE, CTM_UNSIGNED);
    call->type = EXPR_SIZEOF;

    call->sizeof_call.expr = expr;
    return call;
}

void expression_call_free(Expression *expr)
{
    if (!expr || expr->type != EXPR_CALL)
        return;
    c_type_info_free(expr->type_info);
    expression_free(expr->call.func);
    if (expr->call.args)
    {
        for (size_t idx = 0; idx < expr->call.args->size; ++idx)
            expression_free(*((Expression **)vector_get(expr->call.args, idx)));
        vector_free(expr->call.args);
    }
    free(expr);
}