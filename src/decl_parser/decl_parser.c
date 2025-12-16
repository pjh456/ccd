#include "decl_parser.h"
#include "decl_parser_impl/decl_parser_impl.h"
#include "decl_parser_impl/declarator.h"
#include "decl_parser_impl/decl_unit.h"
#include "unit_scanner_impl/statement_unit.h"
#include "tokenizer_impl/token.h"
#include "vector.h"
#include <stdlib.h>

Token *peek_token_in_stmt(StatementUnit *stmt, size_t pos)
{
    if (!stmt || !stmt->tokens || stmt->tokens->size <= pos)
        return NULL;
    return (Token *)vector_get(stmt->tokens, pos);
}

DeclParser *decl_parser_new(Vector *stmts)
{
    DeclParser *p = malloc(sizeof(*p));
    p->stmts = stmts;
    p->stmt_pos = 0;
    p->token_pos = 0;
    return p;
}

void decl_parser_free(DeclParser *dp)
{
    if (!dp)
        return;
    if (dp->stmts)
    {
        for (size_t idx = 0; idx < dp->stmts->size; ++idx)
            statement_unit_free(*((StatementUnit **)vector_get(dp->stmts, idx)));
        vector_free(dp->stmts);
    }
    free(dp);
}

StatementUnit *peek_statement(DeclParser *dp)
{
    return *((StatementUnit **)vector_get(dp->stmts, dp->stmt_pos));
}
StatementUnit *next_statement(DeclParser *dp)
{
    dp->token_pos = 0;
    return *((StatementUnit **)vector_get(dp->stmts, dp->stmt_pos++));
}

Vector *parse_file_decl(DeclParser *dp)
{
    if (!dp)
        return NULL;

    Vector *units = vector_new(sizeof(DeclUnit *));
    StatementUnit *stmt = peek_statement(dp);
    while (stmt)
    {
        DeclUnit *unit = NULL;
        if (is_declaration_statement(stmt))
            unit = parse_decl_statement(dp);
        else
        {
            if (stmt->type == SUT_DECL_OR_EXPR)
                unit = make_expression_decl_unit(stmt);
            else
                unit = make_statement_decl_unit(stmt);
        }
        vector_push_back(units, &unit);
        next_statement(dp);
        stmt = peek_statement(dp);
    }

    return units;
}

int is_declaration_statement(StatementUnit *stmt)
{
    if (!stmt || stmt->type != SUT_DECL_OR_EXPR ||
        !stmt->tokens || !stmt->tokens)
        return 0;
    Token *t = peek_token_in_stmt(stmt, 0);

    switch (t->type)
    {
    case T_IDENTIFIER:
        if (is_in_sue_table(t->str))
            return 1;
        else if (is_in_typedef_table(t->str))
            return 1;
        else
            return 0;
    case T_EXTERN:
    case T_STATIC:
    case T_REGISTER:
    case T_TYPEDEF:

    case T_INLINE:

    case T_CONST:
    case T_VOLATILE:
    case T_RESTRICT:

    case T_AUTO:
    case T_VOID:
    case T_SIGNED:
    case T_UNSIGNED:
    case T_CHAR:
    case T_SHORT:
    case T_INT:
    case T_LONG:
    case T_FLOAT:
    case T_DOUBLE:

    case T_STRUCT:
    case T_UNION:
    case T_ENUM:
        return 1;
    default:
        return 0;
    }
}

DeclUnit *parse_decl_statement(DeclParser *dp)
{
    if (!dp)
        return NULL;
    StatementUnit *stmt = peek_statement(dp);
    if (!stmt || stmt->type != SUT_DECL_OR_EXPR ||
        !stmt->tokens || !is_declaration_statement(stmt))
        return NULL;

    DeclSpecifier *spec = parse_decl_specifier(dp);
    Vector *decls = parse_decl_initializer_list(dp);
    DeclUnit *unit = make_declaration_decl_unit(spec, decls);
    return unit;
}
