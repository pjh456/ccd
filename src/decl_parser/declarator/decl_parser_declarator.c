#include "decl_parser.h"
#include "decl_parser_impl/decl_parser_impl.h"
#include "decl_parser_impl/declarator.h"
#include "decl_parser_impl/declarator_impl/decl_initializer.h"
#include "decl_parser_impl/declarator_impl/decl_param.h"
#include "decl_parser_impl/decl_specifier.h"
#include "unit_scanner_impl/statement_unit.h"
#include "decl_parser_impl/decl_unit.h"
#include "tokenizer_impl/token.h"
#include "vector.h"
#include "utils.h"
#include <stdlib.h>

Vector *parse_decl_initializer_list(DeclParser *dp)
{
    if (!dp)
        return NULL;
    StatementUnit *stmt = peek_statement(dp);
    if (!stmt || stmt->type != SUT_DECL_OR_EXPR ||
        !stmt->tokens || !is_declaration_statement(stmt))
        return NULL;

    Vector *list = vector_new(sizeof(DeclInitializer *));
    while (1)
    {
        DeclInitializer *init = parse_decl_initializer(dp);
        if (!init)
            break;
        vector_push_back(list, &init);
    }

    return list;
}

DeclInitializer *parse_decl_initializer(DeclParser *dp)
{
    if (!dp)
        return NULL;
    StatementUnit *stmt = peek_statement(dp);
    if (!stmt || stmt->type != SUT_DECL_OR_EXPR ||
        !stmt->tokens || !is_declaration_statement(stmt))
        return NULL;

    Declarator *decl = parse_declarator(dp);

    DeclInitializer *di = NULL;
    Token *t = peek_token_in_stmt(stmt, dp->token_pos);
    if (t->type == T_EQUAL)
    {
        t = peek_token_in_stmt(stmt, ++dp->token_pos);
        size_t init_start_pos = dp->token_pos;

        while (t)
        {
            if (t->type == T_COMMA)
                break;
            else if (t->type == T_SEMICOLON)
                break;
            t = peek_token_in_stmt(stmt, ++dp->token_pos);
        }
        if (init_start_pos != dp->token_pos)
            di = make_decl_initializer(
                decl,
                make_expression_decl_unit(
                    make_decl_or_expr_statement_unit(
                        vector_slice(
                            stmt->tokens,
                            init_start_pos, dp->token_pos))));
        else
            di = make_decl_initializer(decl, NULL);
    }
    else
        di = make_decl_initializer(decl, NULL);

    return di;
}

Declarator *parse_declarator(DeclParser *dp)
{
    if (!dp)
        return NULL;
    StatementUnit *stmt = peek_statement(dp);
    if (!stmt || stmt->type != SUT_DECL_OR_EXPR ||
        !stmt->tokens || !is_declaration_statement(stmt))
        return NULL;

    Declarator *decl = NULL;

    // perfix
    Token *t = peek_token_in_stmt(stmt, dp->token_pos);
    switch (t->type)
    {
    case T_STAR:
    {
        t = peek_token_in_stmt(stmt, ++dp->token_pos);
        unsigned qualifier = DTQ_NONE;
        while (t)
        {
            if (t->type == T_CONST)
                qualifier |= DTQ_CONST;
            else if (t->type == T_VOLATILE)
                qualifier |= DTQ_VOLATILE;
            else if (t->type == T_RESTRICT)
                qualifier |= DTQ_RESTRICT;
            else
                break;
            t = peek_token_in_stmt(stmt, ++dp->token_pos);
        }
        Declarator *inner = parse_declarator(dp);
        decl = make_pointer_declarator(inner, qualifier);
    }
    break;
    case T_LEFT_PAREN:
        dp->token_pos++;
        decl = parse_declarator(dp);
        if (peek_token_in_stmt(stmt, dp->token_pos)->type != T_RIGHT_PAREN)
        {
            declarator_free(decl);
            return NULL;
        }
        else
            dp->token_pos++;
        break;
    case T_IDENTIFIER:
    {
        char *str = str_n_clone(t->str, t->length);
        decl = make_identifier_declarator(str);
        dp->token_pos++;
    }
    break;
    default:
        return NULL;
    }

    // suffix
    t = peek_token_in_stmt(stmt, dp->token_pos);
    while (t)
    {
        switch (t->type)
        {
        case T_LEFT_BRACKET:
        {
            size_t len_start_pos = ++dp->token_pos;

            t = peek_token_in_stmt(stmt, dp->token_pos);
            while (t && t->type != T_RIGHT_BRACKET)
                t = peek_token_in_stmt(stmt, ++dp->token_pos);

            Declarator *outer = make_array_declarator(decl, NULL);
            if (len_start_pos != dp->token_pos)
                outer->array.length = make_expression_decl_unit(
                    make_decl_or_expr_statement_unit(
                        vector_slice(
                            stmt->tokens,
                            len_start_pos, dp->token_pos + 1)));

            return outer;
        }
        case T_LEFT_PAREN:
        {
            t = peek_token_in_stmt(stmt, ++dp->token_pos);
            // int is_variadic = 0; // TODO

            Vector *params = parse_decl_param_list(dp);

            if (peek_token_in_stmt(stmt, dp->token_pos)->type != T_RIGHT_PAREN)
            {
                declarator_free(decl);
                return NULL;
            }
            else
                dp->token_pos++;

            Declarator *outer = make_function_declarator(
                decl, params, 0);
            return outer;
        }
        default:
            break;
        }
    }

    return decl;
}

Vector *parse_decl_param_list(DeclParser *dp)
{
    if (!dp)
        return NULL;
    StatementUnit *stmt = peek_statement(dp);
    if (!stmt || stmt->type != SUT_DECL_OR_EXPR ||
        !stmt->tokens || !is_declaration_statement(stmt))
        return NULL;

    Token *t = peek_token_in_stmt(stmt, dp->token_pos);
    Vector *params = vector_new(sizeof(DeclParam *));
    if (t->type == T_RIGHT_PAREN)
        return params;

    while (t)
    {
        DeclParam *param = parse_decl_param(dp);
        vector_push_back(params, &param);

        t = peek_token_in_stmt(stmt, ++dp->token_pos);
        if (t->type != T_COMMA)
            break;
    }

    return params;
}

DeclParam *parse_decl_param(DeclParser *dp)
{
    if (!dp)
        return NULL;
    StatementUnit *stmt = peek_statement(dp);
    if (!stmt || stmt->type != SUT_DECL_OR_EXPR ||
        !stmt->tokens || !is_declaration_statement(stmt))
        return NULL;

    DeclSpecifier *spec = parse_decl_specifier(dp);
    if (!spec)
        return NULL;

    Declarator *decl = parse_declarator(dp);

    DeclParam *param = NULL;
    make_decl_param(spec, decl);

    return param;
}