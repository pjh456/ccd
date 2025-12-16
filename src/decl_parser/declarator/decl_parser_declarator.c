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
    Token *t = peek_token_in_stmt(stmt, dp->token_pos);
    while (1)
    {
        DeclInitializer *init = parse_decl_initializer(dp);
        if (!init)
            break;
        vector_push_back(list, &init);

        t = peek_token_in_stmt(stmt, dp->token_pos);
        if (!t)
            break;

        if (t->type == T_SEMICOLON)
            break;
        else if (t->type == T_COMMA)
            dp->token_pos++;
        else
        {
            for (size_t idx = 0; idx < list->size; ++idx)
                decl_initializer_free(
                    *((DeclInitializer **)vector_get(list, idx)));
            vector_free(list);
            return NULL;
        }
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
    if (t && t->type == T_EQUAL)
    {
        t = peek_token_in_stmt(stmt, ++dp->token_pos);
        size_t init_start_pos = dp->token_pos;
        size_t paren_depth = 0;
        size_t brace_depth = 0;

        while (t)
        {
            if (t->type == T_LEFT_BRACE)
                brace_depth++;
            else if (t->type == T_RIGHT_BRACE)
                brace_depth--;
            else if (t->type == T_LEFT_PAREN)
                paren_depth++;
            else if (t->type == T_RIGHT_PAREN)
                paren_depth--;
            else if (paren_depth == 0 && brace_depth == 0)
            {
                if (t->type == T_COMMA)
                    break;
                else if (t->type == T_SEMICOLON)
                    break;
            }
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

    Vector *ptr_buffer = vector_new(sizeof(unsigned));

    Token *t = peek_token_in_stmt(stmt, dp->token_pos);
    while (t && t->type == T_STAR)
    {
        dp->token_pos++;
        unsigned qual = parse_pointer_qualifiers(dp);
        vector_push_back(ptr_buffer, &qual);
        t = peek_token_in_stmt(stmt, dp->token_pos);
    }

    Declarator *decl = parse_direct_declarator(dp);
    if (!decl)
        return NULL;

    for (int idx = ptr_buffer->size - 1; idx >= 0; --idx)
        decl = make_pointer_declarator(decl, *((unsigned *)vector_get(ptr_buffer, idx)));

    return decl;
}

// Declarator *parse_pointer(DeclParser *dp)
// {
//     if (!dp)
//         return NULL;
//     StatementUnit *stmt = peek_statement(dp);
//     if (!stmt || stmt->type != SUT_DECL_OR_EXPR ||
//         !stmt->tokens || !is_declaration_statement(stmt))
//         return NULL;
// }

Declarator *parse_direct_declarator(DeclParser *dp)
{
    if (!dp)
        return NULL;
    StatementUnit *stmt = peek_statement(dp);
    if (!stmt || stmt->type != SUT_DECL_OR_EXPR ||
        !stmt->tokens || !is_declaration_statement(stmt))
        return NULL;

    Declarator *decl = NULL;
    Token *t = peek_token_in_stmt(stmt, dp->token_pos);
    if (!t)
        return NULL;

    if (t->type == T_IDENTIFIER)
    {
        char *str = str_n_clone(t->str, t->length);
        decl = make_identifier_declarator(str);
        free(str);
        dp->token_pos++;
    }
    else if (t->type == T_LEFT_PAREN)
    {
        dp->token_pos++;
        decl = parse_declarator(dp);
        t = peek_token_in_stmt(stmt, dp->token_pos);
        if (!t || t->type != T_RIGHT_PAREN)
        {
            declarator_free(decl);
            return NULL;
        }
        dp->token_pos++;
    }
    else
        return NULL;

    while (1)
    {
        t = peek_token_in_stmt(stmt, dp->token_pos);
        if (!t)
            break;

        if (t->type == T_LEFT_BRACKET)
            decl = parse_array_suffix(dp, decl);
        else if (t->type == T_LEFT_PAREN)
            decl = parse_function_suffix(dp, decl);
        else
            break;
    }

    return decl;
}

unsigned parse_pointer_qualifiers(DeclParser *dp)
{
    if (!dp)
        return DTQ_NONE;
    StatementUnit *stmt = peek_statement(dp);
    if (!stmt || stmt->type != SUT_DECL_OR_EXPR ||
        !stmt->tokens || !is_declaration_statement(stmt))
        return DTQ_NONE;

    Token *t = peek_token_in_stmt(stmt, dp->token_pos);
    if (!t)
        return DTQ_NONE;

    int is_qualifier_end = 0;
    unsigned qualifiers = DTQ_NONE;
    while (t)
    {
        switch (t->type)
        {
        case T_CONST:
            qualifiers |= DTQ_CONST;
            break;
        case T_VOLATILE:
            qualifiers |= DTQ_VOLATILE;
            break;
        case T_RESTRICT:
            qualifiers |= DTQ_RESTRICT;
            break;
        default:
            is_qualifier_end = 1;
            break;
        }
        if (is_qualifier_end)
            break;
        t = peek_token_in_stmt(stmt, ++dp->token_pos);
    }

    return qualifiers;
}

Declarator *parse_array_suffix(DeclParser *dp, Declarator *decl)
{
    if (!dp)
        return NULL;
    StatementUnit *stmt = peek_statement(dp);
    if (!stmt || stmt->type != SUT_DECL_OR_EXPR ||
        !stmt->tokens || !is_declaration_statement(stmt))
        return NULL;

    Token *t = peek_token_in_stmt(stmt, dp->token_pos);
    if (!t || t->type != T_LEFT_BRACKET)
        return NULL;

    size_t len_start = dp->token_pos;
    size_t depth = 0;
    t = peek_token_in_stmt(stmt, ++dp->token_pos);
    while (t)
    {
        if (t->type == T_LEFT_BRACKET)
            depth++;
        else if (t->type == T_RIGHT_BRACKET)
        {
            if (depth == 0)
                break;
            else
                depth--;
        }
        t = peek_token_in_stmt(stmt, ++dp->token_pos);
    }

    if (!t || t->type != T_RIGHT_BRACKET)
        return NULL;
    dp->token_pos++;

    Declarator *outer = make_array_declarator(decl, NULL);

    if (len_start < dp->token_pos - 1)
        outer->array.length =
            make_expression_decl_unit(
                make_decl_or_expr_statement_unit(
                    vector_slice(stmt->tokens, len_start, dp->token_pos)));

    return outer;
}

Declarator *parse_function_suffix(DeclParser *dp, Declarator *decl)
{
    if (!dp)
        return NULL;
    StatementUnit *stmt = peek_statement(dp);
    if (!stmt || stmt->type != SUT_DECL_OR_EXPR ||
        !stmt->tokens || !is_declaration_statement(stmt))
        return NULL;

    Token *t = peek_token_in_stmt(stmt, dp->token_pos);
    if (!t || t->type != T_LEFT_PAREN)
        return NULL;

    Vector *params = vector_new(sizeof(DeclParam *));
    int is_variadic = 0;
    t = peek_token_in_stmt(stmt, ++dp->token_pos);

    if (t && t->type != T_RIGHT_PAREN)
    {
        while (1)
        {
            if (t->type == T_ELLIPSIS)
            {
                is_variadic = 1;
                dp->token_pos++;
                break;
            }

            DeclParam *param = parse_decl_param(dp);
            if (!param)
            {
                for (size_t idx = 0; idx < params->size; ++idx)
                    decl_param_free(*((DeclParam **)vector_get(params, idx)));
                vector_free(params);
                return NULL;
            }
            vector_push_back(params, &param);

            t = peek_token_in_stmt(stmt, dp->token_pos);
            if (!t || t->type != T_COMMA)
                break;

            t = peek_token_in_stmt(stmt, ++dp->token_pos);
        }
    }

    if (!t || t->type != T_RIGHT_PAREN)
        return NULL;
    dp->token_pos++;

    return make_function_declarator(decl, params, is_variadic);
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
        if (!t || t->type != T_COMMA)
            break;
        t = peek_token_in_stmt(stmt, dp->token_pos + 1);
        if (t && t->type == T_ELLIPSIS)
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

    DeclParam *param = make_decl_param(spec, decl);

    return param;
}