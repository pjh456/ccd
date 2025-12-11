#include "parser_impl/expression.h"
#include "parser_impl/expression_impl.h"
#include "parser_impl/expression_simple_impl.h"
#include "parser_impl/expression_literal_impl.h"
#include "parser_impl/expression_identifier_impl.h"
#include "parser_impl/expression_function_impl.h"
#include "parser_impl/expression_operator_impl.h"
#include "parser_impl/c_type_info.h"
#include "parser_impl/c_type_info_impl.h"
#include "vector.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Expression *expression_copy(Expression *expr)
{
    if (!expr)
        return NULL;
    Expression *copied_expr = malloc(sizeof(*copied_expr));
    memcpy(copied_expr, expr, sizeof(*expr));
    return copied_expr;
}

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
        expression_assign_free(expr);
        break;
    case EXPR_UNARY:
        expression_unary_free(expr);
        break;
    case EXPR_BINARY:
        expression_binary_free(expr);
        break;
    case EXPR_CALL:
        expression_call_free(expr);
        break;
    case EXPR_SIZEOF:
        expression_sizeof_free(expr);
        break;
    case EXPR_SUBSCRIPT:
        expression_subscript_free(expr);
        break;
    case EXPR_MEMBER:
        expression_member_free(expr);
        break;
    case EXPR_PTR_MEMBER:
        expression_ptr_member_free(expr);
        break;
    case EXPR_CONDITIONAL:
        expression_conditional_free(expr);
        break;
    case EXPR_COMMA:
        expression_comma_free(expr);
        break;
    case EXPR_PAREN:
        expression_paren_free(expr);
        break;
    case EXPR_UNKNOWN:
    default:
        free(expr);
        break;
    }
}

static const char *expr_type_name(ExprType t)
{
    switch (t)
    {
    case EXPR_LITERAL:
        return "Literal";
    case EXPR_IDENTIFIER:
        return "Identifier";
    case EXPR_CAST:
        return "Cast";
    case EXPR_ASSIGN:
        return "Assignment";
    case EXPR_UNARY:
        return "UnaryOp";
    case EXPR_BINARY:
        return "BinaryOp";
    case EXPR_CALL:
        return "Call";
    case EXPR_SIZEOF:
        return "Sizeof";
    case EXPR_SUBSCRIPT:
        return "Subscript";
    case EXPR_MEMBER:
        return "Member";
    case EXPR_PTR_MEMBER:
        return "PtrMember";
    case EXPR_CONDITIONAL:
        return "Conditional";
    case EXPR_COMMA:
        return "Comma";
    case EXPR_PAREN:
        return "Paren";
    default:
        return "<unknown>";
    }
}

static const char *assign_op_name(AssignOperator op)
{
    switch (op)
    {
    case OP_ASSIGN:
        return "=";
    case OP_AND_ASSIGN:
        return "&=";
    case OP_OR_ASSIGN:
        return "|=";
    case OP_XOR_ASSIGN:
        return "^=";
    case OP_LEFT_SHIFT_ASSIGN:
        return "<<=";
    case OP_RIGHT_SHIFT_ASSIGN:
        return ">>=";
    case OP_PLUS_ASSIGN:
        return "+=";
    case OP_MINUS_ASSIGN:
        return "-=";
    case OP_MUL_ASSIGN:
        return "*=";
    case OP_DIV_ASSIGN:
        return "/=";
    case OP_MOD_ASSIGN:
        return "%=";
    default:
        return "<unknown assign>";
    }
}

static const char *unary_op_name(UnaryOperator op)
{
    switch (op)
    {
    case OP_NOT:
        return "!";
    case OP_TILDE:
        return "~";
    case OP_INC:
        return "++";
    case OP_DEC:
        return "--";
    default:
        return "<unknown unary>";
    }
}

static const char *binary_op_name(BinaryOperator op)
{
    switch (op)
    {
    case OP_LESS:
        return "<";
    case OP_GREATER:
        return ">";
    case OP_EQUAL:
        return "==";
    case OP_NOT_EQUAL:
        return "!=";
    case OP_LESS_EQUAL:
        return "<=";
    case OP_GREATER_EQUAL:
        return ">=";
    case OP_AND_AND:
        return "&&";
    case OP_OR_OR:
        return "||";
    case OP_AND:
        return "&";
    case OP_OR:
        return "|";
    case OP_XOR:
        return "^";
    case OP_LEFT_SHIFT:
        return "<<";
    case OP_RIGHT_SHIFT:
        return ">>";
    case OP_PLUS:
        return "+";
    case OP_MINUS:
        return "-";
    case OP_STAR:
        return "*";
    case OP_DIV:
        return "/";
    case OP_MOD:
        return "%";
    default:
        return "<unknown binary>";
    }
}

static void print_literal(Expression *expr, int indent)
{
    printf("%*sLiteral: ", indent, "");

    CTypeInfo *t = expr->type_info;
    if (!t)
    {
        printf("<no type>\n");
        return;
    }

    switch (t->type)
    {
    case CT_CHAR:
        printf("'%c'\n", expr->literal.data.signed_char_v);
        break;
    case CT_INT:
        printf("%d\n", expr->literal.data.signed_int_v);
        break;
    case CT_FLOAT:
        printf("%f\n", expr->literal.data.float_v);
        break;
    case CT_DOUBLE:
        printf("%lf\n", expr->literal.data.double_v);
        break;
    case CT_POINTER:
        printf("\"%s\"\n", expr->literal.data.string_v);
        break;
    default:
        printf("<unknown literal>\n");
        break;
    }
}

void print_expression(Expression *expr)
{
    print_expression_impl(expr, 0);
}

void print_expression_impl(Expression *expr, int indent)
{
    if (!expr)
    {
        printf("%*s<null expr>\n", indent, "");
        return;
    }

    printf("%*s%s\n", indent, "", expr_type_name(expr->type));

    switch (expr->type)
    {
    case EXPR_LITERAL:
        print_literal(expr, indent + 2);
        break;

    case EXPR_IDENTIFIER:
        printf("%*sname = %s\n", indent + 2, "", expr->ident.name);
        break;

    case EXPR_CAST:
        printf("%*scast to:\n", indent + 2, "");
        print_c_type_info_impl(expr->cast.type_info, indent + 4);
        printf("%*sexpr:\n", indent + 2, "");
        print_expression_impl(expr->cast.expr, indent + 4);
        break;

    case EXPR_ASSIGN:
        printf("%*soperator = %s\n", indent + 2, "", assign_op_name(expr->assign.op));
        printf("%*slhs:\n", indent + 2, "");
        print_expression_impl(expr->assign.lhs, indent + 4);
        printf("%*srhs:\n", indent + 2, "");
        print_expression_impl(expr->assign.rhs, indent + 4);
        break;

    case EXPR_UNARY:
        printf("%*soperator = %s (%s)\n", indent + 2, "",
               unary_op_name(expr->unary.op),
               expr->unary.is_prefix ? "prefix" : "postfix");
        printf("%*sexpr:\n", indent + 2, "");
        print_expression_impl(expr->unary.expr, indent + 4);
        break;

    case EXPR_BINARY:
        printf("%*soperator = %s\n", indent + 2, "", binary_op_name(expr->binary.op));
        printf("%*slhs:\n", indent + 2, "");
        print_expression_impl(expr->binary.lhs, indent + 4);
        printf("%*srhs:\n", indent + 2, "");
        print_expression_impl(expr->binary.rhs, indent + 4);
        break;

    case EXPR_CALL:
    {
        printf("%*sfunc:\n", indent + 2, "");
        print_expression_impl(expr->call.func, indent + 4);

        printf("%*sargs:\n", indent + 2, "");
        if (!expr->call.args || expr->call.args->size == 0)
        {
            printf("%*s<none>\n", indent + 4, "");
            break;
        }

        for (size_t i = 0; i < expr->call.args->size; ++i)
        {
            Expression *arg = *((Expression **)vector_get(expr->call.args, i));
            print_expression_impl(arg, indent + 4);
        }
        break;
    }

    case EXPR_SIZEOF:
        printf("%*sexpr:\n", indent + 2, "");
        print_expression_impl(expr->sizeof_call.expr, indent + 4);
        break;

    case EXPR_SUBSCRIPT:
        printf("%*sbase:\n", indent + 2, "");
        print_expression_impl(expr->subscript.base, indent + 4);
        printf("%*sindex:\n", indent + 2, "");
        print_expression_impl(expr->subscript.index, indent + 4);
        break;

    case EXPR_MEMBER:
        printf("%*sbase:\n", indent + 2, "");
        print_expression_impl(expr->member.base, indent + 4);
        printf("%*smember:\n", indent + 2, "");
        print_expression_impl(expr->member.mem, indent + 4);
        break;

    case EXPR_PTR_MEMBER:
        printf("%*s(base -> member)\n", indent + 2, "");
        printf("%*sbase:\n", indent + 2, "");
        print_expression_impl(expr->member.base, indent + 4);
        printf("%*smember:\n", indent + 2, "");
        print_expression_impl(expr->member.mem, indent + 4);
        break;

    case EXPR_CONDITIONAL:
        printf("%*scond:\n", indent + 2, "");
        print_expression_impl(expr->conditional.cond, indent + 4);
        printf("%*sthen:\n", indent + 2, "");
        print_expression_impl(expr->conditional.then_expr, indent + 4);
        printf("%*selse:\n", indent + 2, "");
        print_expression_impl(expr->conditional.else_expr, indent + 4);
        break;

    case EXPR_COMMA:
    {
        printf("%*sitems:\n", indent + 2, "");
        if (!expr->comma.exprs || expr->comma.exprs->size == 0)
        {
            printf("%*s<none>\n", indent + 4, "");
            break;
        }

        for (size_t i = 0; i < expr->comma.exprs->size; ++i)
        {
            Expression *e = *((Expression **)vector_get(expr->comma.exprs, i));
            print_expression_impl(e, indent + 4);
        }
        break;
    }

    case EXPR_PAREN:
        printf("%*sexpr:\n", indent + 2, "");
        print_expression_impl(expr->paren.expr, indent + 4);
        break;

    default:
        printf("%*s<unknown expr>\n", indent + 2, "");
        break;
    }

    // optional: expression type
    if (expr->type_info)
    {
        printf("%*stype info:\n", indent + 2, "");
        print_c_type_info_impl(expr->type_info, indent + 4);
    }
}