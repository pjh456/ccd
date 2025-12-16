#include "decl_parser.h"
#include "decl_parser_impl/decl_parser_impl.h"
#include "decl_parser_impl/decl_specifier.h"
#include "unit_scanner_impl/statement_unit.h"
#include "tokenizer_impl/token.h"
#include "vector.h"
#include <stdlib.h>

DeclSpecifier *parse_decl_specifier(DeclParser *dp)
{
    if (!dp)
        return NULL;
    StatementUnit *stmt = peek_statement(dp);
    if (!stmt || stmt->type != SUT_DECL_OR_EXPR || !is_declaration_statement(stmt))
        return NULL;

    unsigned storages = DS_NONE;
    unsigned func_specs = DFS_NONE;
    unsigned qualifiers = DTQ_NONE;
    unsigned modifiers = DTM_NONE;
    DeclBuiltinType builtin_type = DBT_NONE;
    DeclSUE sue_type = DSUE_NONE;

    Token *t = peek_token_in_stmt(stmt, dp->token_pos);
    int is_spec_end = 0;
    while (t)
    {
        t = peek_token_in_stmt(stmt, dp->token_pos);
        switch (t->type)
        {
        case T_EXTERN:
            storages |= DS_EXTERN;
            break;
        case T_STATIC:
            storages |= DS_STATIC;
            break;
        case T_REGISTER:
            storages |= DS_REGISTER;
            break;
        case T_TYPEDEF:
            storages |= DS_TYPEDEF;
            break;

        case T_INLINE:
            func_specs |= DFS_INLINE;
            break;

        case T_CONST:
            qualifiers |= DTQ_CONST;
            break;
        case T_VOLATILE:
            qualifiers |= DTQ_VOLATILE;
            break;
        case T_RESTRICT:
            qualifiers |= DTQ_RESTRICT;
            break;

        case T_SIGNED:
            modifiers |= DTM_SIGNED;
            break;
        case T_UNSIGNED:
            modifiers |= DTM_UNSIGNED;
            break;
        case T_SHORT:
            modifiers |= DTM_SHORT;
            break;
        case T_LONG:
            if (modifiers & DTM_LONG)
                modifiers |= DTM_LONGLONG;
            else
                modifiers |= DTM_LONG;
            break;

        case T_VOID:
            builtin_type = DBT_VOID;
            break;
        case T_CHAR:
            builtin_type = DBT_CHAR;
            break;
        case T_INT:
            builtin_type = DBT_INT;
            break;
        case T_FLOAT:
            builtin_type = DBT_FLOAT;
            break;
        case T_DOUBLE:
            builtin_type = DBT_DOUBLE;
            break;

        case T_STRUCT:
            sue_type = DSUE_STRUCT;
            break;
        case T_UNION:
            sue_type = DSUE_UNION;
            break;
        case T_ENUM:
            sue_type = DSUE_ENUM;
            break;

        default:
            is_spec_end = 1;
            break;
        }

        if (is_spec_end)
            break;
        dp->token_pos++;
    }

    if (builtin_type == DBT_NONE && sue_type == DSUE_NONE)
        builtin_type = DBT_INT;

    DeclSpecifier *spec = make_decl_specifier(
        builtin_type, sue_type,
        storages, func_specs,
        qualifiers, modifiers);

    return spec;
}