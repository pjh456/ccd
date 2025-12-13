#include "unit_scanner.h"
#include "unit_scanner_impl/unit_scanner_impl.h"
#include "unit_scanner_impl/statement_unit.h"
#include "tokenizer_impl/token.h"
#include "vector.h"
#include "utils.h"

StatementUnit *scan_decl_or_expression(UnitScanner *us, int no_semicolon)
{
    if (!us)
        return NULL;
    switch (peek_token(us)->type)
    {
    case T_SEMICOLON:
    case T_IDENTIFIER:
    case T_LEFT_PAREN:
    case T_NUMBER:
    case T_CHARACTER:
    case T_STRING:

    case T_NOT:
    case T_TILDE:
    case T_MINUS:

    case T_AND:
    case T_STAR:

    case T_INC:
    case T_DEC:

    case T_EXTERN:
    case T_STATIC:
    case T_INLINE:
    case T_REGISTER:
    case T_RESTRICT:
    case T_VOLATILE:
    case T_CONST:

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

    case T_DO:
    case T_WHILE:
    case T_FOR:
    case T_CONTINUE:
    case T_BREAK:
    case T_IF:
    case T_ELSE:
    case T_SWITCH:
    case T_CASE:
    case T_DEFAULT:
    case T_RETURN:
    case T_GOTO:

    case T_ENUM:
    case T_SIZEOF:
    case T_STRUCT:
    case T_TYPEDEF:
    case T_UNION:
        break;
    default:
        return NULL;
    }

    size_t pos = us->pos;
    size_t depth = 0;
    while (peek_token(us)->type != T_EOF)
    {
        if (no_semicolon)
        {
            if (peek_token(us)->type == T_LEFT_PAREN)
                depth++;
            else if (peek_token(us)->type == T_RIGHT_PAREN)
            {
                if (depth)
                    depth--;
                else
                    break;
            }
            else if (peek_token(us)->type == T_COLON)
                break;
            else if (peek_token(us)->type == T_SEMICOLON)
                break;
        }
        else
        {
            if (peek_token(us)->type == T_SEMICOLON)
            {
                next_token(us);
                break;
            }
        }
        next_token(us);
    }

    StatementUnit *unit = make_decl_or_expr_statement_unit(
        vector_slice(us->tokens, pos, us->pos));

    return unit;
}