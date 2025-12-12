#include "unit_scanner_impl/statement_unit.h"
#include "unit_scanner_impl/statement_unit_impl.h"
#include "unit_scanner_impl/statement_unit_compound_impl.h"
#include "unit_scanner_impl/statement_unit_declaration_impl.h"
#include "unit_scanner_impl/statement_unit_conditional_impl.h"
#include "unit_scanner_impl/statement_unit_loop_impl.h"
#include "unit_scanner_impl/statement_unit_break_impl.h"
#include "unit_scanner_impl/statement_unit_label_impl.h"
#include "tokenizer_impl/token.h"
#include "tokenizer_impl/tokenizer_impl.h"
#include "vector.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>

void statement_unit_free(StatementUnit *unit)
{
    if (!unit)
        return;

    switch (unit->type)
    {
    case SUT_COMPOUND:
        statement_unit_compound_free(unit);
        break;
    case SUT_EMPTY:
        statement_unit_empty_free(unit);
        break;
    case SUT_DECL:
        statement_unit_declaration_free(unit);
        break;
    case SUT_EXPR:
        statement_unit_expression_free(unit);
        break;
    case SUT_IF:
        statement_unit_if_free(unit);
        break;
    case SUT_SWITCH:
        statement_unit_switch_free(unit);
        break;
    case SUT_CASE:
        statement_unit_case_free(unit);
        break;
    case SUT_DEFAULT:
        statement_unit_default_free(unit);
        break;
    case SUT_WHILE:
        statement_unit_while_free(unit);
        break;
    case SUT_DO_WHILE:
        statement_unit_do_while_free(unit);
        break;
    case SUT_FOR:
        statement_unit_for_free(unit);
        break;
    case SUT_CONTINUE:
        statement_unit_continue_free(unit);
        break;
    case SUT_BREAK:
        statement_unit_break_free(unit);
        break;
    case SUT_RETURN:
        statement_unit_return_free(unit);
        break;
    case SUT_LABEL:
        statement_unit_label_free(unit);
        break;
    case SUT_GOTO:
        statement_unit_goto_free(unit);
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

char *statement_unit_name(StatementUnitType sut)
{
    switch (sut)
    {
    case SUT_COMPOUND:
        return "COMPOUND";
    case SUT_EMPTY:
        return "EMPTY";
    case SUT_DECL:
        return "DECLARATION";
    case SUT_EXPR:
        return "EXPRESSION";
    case SUT_IF:
        return "IF";
    case SUT_SWITCH:
        return "SWITCH";
    case SUT_CASE:
        return "CASE";
    case SUT_DEFAULT:
        return "DEFAULT";
    case SUT_WHILE:
        return "WHILE";
    case SUT_DO_WHILE:
        return "DO_WHILE";
    case SUT_FOR:
        return "FOR";
    case SUT_CONTINUE:
        return "CONTINUE";
    case SUT_BREAK:
        return "BREAK";
    case SUT_RETURN:
        return "RETURN";
    case SUT_LABEL:
        return "LABEL";
    case SUT_GOTO:
        return "GOTO";
    default:
        return "UNKNOWN";
    }
}

void print_statement_unit(StatementUnit *unit)
{
    print_statement_unit_impl(unit, 0);
}

void print_tokens(Vector *tokens)
{
    if (!tokens)
    {
        printf("<no tokens>");
        return;
    }

    for (size_t i = 0; i < tokens->size; ++i)
    {
        Token *t = vector_get(tokens, i);
        printf("%.*s ", (int)(t->length), t->start);
    }
}

void print_statement_unit_impl(StatementUnit *unit, int indent)
{
    switch (unit->type)
    {
    case SUT_COMPOUND:
    {
        Vector *items = unit->compound_stmt.units;
        if (items)
        {
            for (size_t i = 0; i < items->size; ++i)
            {
                StatementUnit *child = vector_get(items, i);
                print_statement_unit_impl(child, indent + 4);
            }
        }
        break;
    }

    case SUT_IF:
        print_indent(indent + 2);
        printf("Condition:\n");
        print_statement_unit_impl(unit->if_stmt.cond, indent + 4);

        print_indent(indent + 2);
        printf("Then:\n");
        print_statement_unit_impl(unit->if_stmt.then_body, indent + 4);

        if (unit->if_stmt.else_body)
        {
            print_indent(indent + 2);
            printf("Else:\n");
            print_statement_unit_impl(unit->if_stmt.else_body, indent + 4);
        }
        break;

    case SUT_SWITCH:
        print_indent(indent + 2);
        printf("Switch expr:\n");
        print_statement_unit_impl(unit->switch_stmt.expr, indent + 4);

        print_indent(indent + 2);
        printf("Body:\n");
        print_statement_unit_impl(unit->switch_stmt.body, indent + 4);
        break;

    case SUT_CASE:
        print_indent(indent + 2);
        printf("Case expr:\n");
        print_statement_unit_impl(unit->case_stmt.expr, indent + 4);
        break;

    case SUT_WHILE:
        print_indent(indent + 2);
        printf("Condition:\n");
        print_statement_unit_impl(unit->while_stmt.cond, indent + 4);

        print_indent(indent + 2);
        printf("Body:\n");
        print_statement_unit_impl(unit->while_stmt.body, indent + 4);
        break;

    case SUT_DO_WHILE:
        print_indent(indent + 2);
        printf("Body:\n");
        print_statement_unit_impl(unit->do_while_stmt.body, indent + 4);

        print_indent(indent + 2);
        printf("Condition:\n");
        print_statement_unit_impl(unit->do_while_stmt.cond, indent + 4);
        break;

    case SUT_FOR:
        print_indent(indent + 2);
        printf("Init:\n");
        print_statement_unit_impl(unit->for_stmt.init, indent + 4);

        print_indent(indent + 2);
        printf("Cond:\n");
        print_statement_unit_impl(unit->for_stmt.cond, indent + 4);

        print_indent(indent + 2);
        printf("Step:\n");
        print_statement_unit_impl(unit->for_stmt.step, indent + 4);

        print_indent(indent + 2);
        printf("Body:\n");
        print_statement_unit_impl(unit->for_stmt.body, indent + 4);
        break;

    case SUT_RETURN:
        if (unit->return_stmt.expr)
        {
            print_indent(indent + 2);
            printf("Expr:\n");
            print_statement_unit_impl(unit->return_stmt.expr, indent + 4);
        }
        break;

    case SUT_LABEL:
        print_indent(indent + 2);
        printf("Label name: %s\n", unit->label_stmt.name);
        break;

    case SUT_GOTO:
        print_indent(indent + 2);
        printf("Goto name: %s\n", unit->goto_stmt.name);
        break;

    // 简单语句：Decl, Expr, Break, Continue, Empty, Default
    case SUT_DECL:
    case SUT_EXPR:
    case SUT_BREAK:
    case SUT_CONTINUE:
    case SUT_EMPTY:
    case SUT_DEFAULT:
        // 已打印 token，结构无子节点
        break;

    default:
        print_indent(indent + 2);
        printf("<unknown variant>\n");
    }
}