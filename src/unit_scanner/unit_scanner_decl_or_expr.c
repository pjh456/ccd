#include "unit_scanner.h"
#include "unit_scanner_impl/unit_scanner_impl.h"
#include "unit_scanner_impl/statement_unit.h"
#include "tokenizer_impl/token.h"
#include "vector.h"
#include "utils.h"

StatementUnit *scan_decl_or_expression(UnitScanner *us)
{
    if (!us)
        return NULL;

    size_t start = us->pos;
    size_t depth = 0;

    while (peek_token(us)->type != T_EOF)
    {
        TokenType t = peek_token(us)->type;

        if (t == T_LEFT_PAREN)
            depth++;
        else if (t == T_RIGHT_PAREN)
        {
            if (depth > 0)
                depth--;
            else
                break;
        }
        else if (depth == 0)
        {
            if (t == T_SEMICOLON || t == T_LEFT_BRACE || t == T_COLON)
                break;
        }

        next_token(us);
    }

    StatementUnit *unit = make_decl_or_expr_statement_unit(
        vector_slice(us->tokens, start, us->pos));

    return unit;
}