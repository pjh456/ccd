#include "parser_impl/expression_literal_impl.h"
#include "parser_impl/expression.h"
#include "parser_impl/c_type_info.h"
#include <stdlib.h>
#include <string.h>

Expression *make_literal(CTypeInfo *cti, void *data)
{
    if (!cti)
        return NULL;

    Expression *expr = malloc(sizeof(*expr));
    expr->literal.type_info = cti;
    expr->type = EXPR_LITERAL;

    switch (cti->type)
    {
    case CT_CHAR:
        if ((cti->modifiers & CTM_UNSIGNED) == 0)
            build_signed_char_literal(expr, *((signed char *)data));
        else
            build_unsigned_char_literal(expr, *((unsigned char *)data));
        break;
    case CT_INT:
        if ((cti->modifiers & CTM_LONGLONG))
        {
            if ((cti->modifiers & CTM_UNSIGNED) == 0)
                build_signed_long_long_literal(expr, *((signed long long *)data));
            else
                build_unsigned_long_long_literal(expr, *((unsigned long long *)data));
        }
        else if ((cti->modifiers & CTM_LONG))
        {
            if ((cti->modifiers & CTM_UNSIGNED) == 0)
                build_signed_long_literal(expr, *((signed long *)data));
            else
                build_unsigned_long_literal(expr, *((unsigned long *)data));
        }
        else if ((cti->modifiers & CTM_SHORT))
        {
            if ((cti->modifiers & CTM_UNSIGNED) == 0)
                build_signed_short_literal(expr, *((signed short *)data));
            else
                build_unsigned_short_literal(expr, *((unsigned short *)data));
        }
        else
        {
            if ((cti->modifiers & CTM_UNSIGNED) == 0)
                build_signed_int_literal(expr, *((signed int *)data));
            else
                build_unsigned_int_literal(expr, *((unsigned int *)data));
        }
        break;
    case CT_FLOAT:
        build_float_literal(expr, *((float *)data));
        break;
    case CT_DOUBLE:
        if ((cti->modifiers & CTM_LONG) != 0)
            build_long_double_literal(expr, *((long double *)data));
        else
            build_double_literal(expr, *((double *)data));
        break;
    case CT_UNKNOWN: // 将其视为 string
        build_string_literal(expr, (char *)data);
        break;
    default:
        free(expr);
        return NULL;
    }

    return expr;
}

void build_signed_char_literal(Expression *expr, signed char value)
{
    if (!expr || expr->type != EXPR_LITERAL)
        return;
    expr->literal.data.signed_char_v = value;
}

void build_unsigned_char_literal(Expression *expr, unsigned char value)
{
    if (!expr || expr->type != EXPR_LITERAL)
        return;
    expr->literal.data.unsigned_char_v = value;
}

void build_signed_short_literal(Expression *expr, signed short value)
{
    if (!expr || expr->type != EXPR_LITERAL)
        return;
    expr->literal.data.signed_short_v = value;
}

void build_unsigned_short_literal(Expression *expr, unsigned short value)
{
    if (!expr || expr->type != EXPR_LITERAL)
        return;
    expr->literal.data.unsigned_short_v = value;
}

void build_signed_int_literal(Expression *expr, signed int value)
{
    if (!expr || expr->type != EXPR_LITERAL)
        return;
    expr->literal.data.signed_int_v = value;
}

void build_unsigned_int_literal(Expression *expr, unsigned int value)
{
    if (!expr || expr->type != EXPR_LITERAL)
        return;
    expr->literal.data.unsigned_int_v = value;
}

void build_signed_long_literal(Expression *expr, signed long value)
{
    if (!expr || expr->type != EXPR_LITERAL)
        return;
    expr->literal.data.signed_long_v = value;
}

void build_unsigned_long_literal(Expression *expr, unsigned long value)
{
    if (!expr || expr->type != EXPR_LITERAL)
        return;
    expr->literal.data.unsigned_long_v = value;
}

void build_signed_long_long_literal(Expression *expr, signed long long value)
{
    if (!expr || expr->type != EXPR_LITERAL)
        return;
    expr->literal.data.signed_long_long_v = value;
}

void build_unsigned_long_long_literal(Expression *expr, unsigned long long value)
{
    if (!expr || expr->type != EXPR_LITERAL)
        return;
    expr->literal.data.unsigned_long_long_v = value;
}

void build_float_literal(Expression *expr, float value)
{
    if (!expr || expr->type != EXPR_LITERAL)
        return;
    expr->literal.data.float_v = value;
}

void build_double_literal(Expression *expr, double value)
{
    if (!expr || expr->type != EXPR_LITERAL)
        return;
    expr->literal.data.double_v = value;
}

void build_long_double_literal(Expression *expr, long double value)
{
    if (!expr || expr->type != EXPR_LITERAL)
        return;
    expr->literal.data.long_double_v = value;
}

void build_string_literal(Expression *expr, char *value)
{
    if (!expr || expr->type != EXPR_LITERAL)
        return;
    expr->literal.data.string_v = malloc(strlen(value) + 1);
    strcpy(expr->literal.data.string_v, value);
}

void expression_literal_free(Expression *expr)
{
    if (!expr || expr->type != EXPR_LITERAL)
        return;
    switch (expr->literal.type_info->type)
    {
    case CT_UNKNOWN: // 字符串
        if (expr->literal.data.string_v)
            free(expr->literal.data.string_v);
    case CT_CHAR:
    case CT_INT:
    case CT_FLOAT:
    case CT_DOUBLE:
    default:
        break;
    }
    free(expr);
}