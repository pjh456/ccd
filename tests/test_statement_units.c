#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "tokenizer.h"
#include "vector.h"
#include "unit_scanner_impl/statement_unit.h"

// ===============================
// TokenStream
// ===============================

typedef struct
{
    Vector *all_tokens;
    size_t cursor;
} TokenStream;

TokenStream init_stream(const char *src)
{
    Tokenizer *tk = tokenizer_new(src);
    Vector *vec = vector_new(sizeof(Token));

    while (1)
    {
        Token *t = next(tk);
        vector_push_back(vec, t);
        if (t->type == T_EOF)
            break;
    }
    tokenizer_free(tk);

    return (TokenStream){vec, 0};
}

Vector *slice_until(TokenStream *ts, TokenType type)
{
    size_t start = ts->cursor;
    size_t end = start;

    while (1)
    {
        Token *t = vector_get(ts->all_tokens, end);
        if (!t)
        {
            fprintf(stderr, "slice_until: unexpected EOF\n");
            exit(1);
        }
        if (t->type == type)
            break;
        end++;
    }

    Vector *sub = vector_slice(ts->all_tokens, start, end + 1);
    ts->cursor = end + 1;
    return sub;
}

Vector *slice_exact(TokenStream *ts, size_t n)
{
    if (ts->cursor + n > ts->all_tokens->size)
    {
        fprintf(stderr, "slice_exact: OOB\n");
        exit(1);
    }
    Vector *sub = vector_slice(ts->all_tokens, ts->cursor, ts->cursor + n);
    ts->cursor += n;
    return sub;
}

// ===============================
//  构造语句：统一策略
// ===============================
//
// 任何语句：
//     获取完整 token 区间 → 交给构造函数
//
// ===============================

void test_full()
{
    printf("=== Full StatementUnit Coverage ===\n");

    const char *src =
        "{"
        "  int a = 1;"
        "  a++;"
        "  if (a) { return a; } else { goto label; }"
        "  while(1) continue;"
        "  do { break; } while(0);"
        "  for(;;);"
        "  switch(a) { case 1: a=2; default: ; }"
        "  label: ;"
        "}";

    TokenStream ts = init_stream(src);

    // root {
    Vector *root_tok = slice_exact(&ts, 1);
    Vector *root_children = vector_new(sizeof(StatementUnit *));

    // -------------------------
    // declaration: int a = 1;
    // -------------------------
    {
        Vector *toks = slice_until(&ts, T_SEMICOLON);
        StatementUnit *u = make_decl_or_expr_statement_unit(toks);
        vector_push_back(root_children, &u);
    }

    // -------------------------
    // expression: a++;
    // -------------------------
    {
        Vector *toks = slice_until(&ts, T_SEMICOLON);
        StatementUnit *u = make_decl_or_expr_statement_unit(toks);
        vector_push_back(root_children, &u);
    }

    // -------------------------
    // if (...) { ... } else { ... }
    // -------------------------
    {
        Vector *if_kw = slice_exact(&ts, 1); // if

        Vector *cond = slice_until(&ts, T_RIGHT_PAREN); // (a)
        StatementUnit *cond_u = make_decl_or_expr_statement_unit(cond);

        // then { return a; }
        Vector *then_l = slice_exact(&ts, 1); // {
        Vector *then_children = vector_new(sizeof(StatementUnit *));
        {
            Vector *ret_kw = slice_exact(&ts, 1);             // return
            Vector *ret_expr = slice_until(&ts, T_SEMICOLON); // a;
            StatementUnit *ret_u = make_return_statement_unit(
                ret_kw,
                make_decl_or_expr_statement_unit(ret_expr));
            vector_push_back(then_children, &ret_u);
        }
        Vector *then_r = slice_exact(&ts, 1); // }

        vector_push_back(then_l, vector_get(then_r, 0));
        vector_free(then_r);

        StatementUnit *then_u = make_compound_statement_unit(then_l, then_children);

        // else { goto label; }
        Vector *else_kw = slice_exact(&ts, 1); // else
        (void)else_kw;

        Vector *else_l = slice_exact(&ts, 1); // {
        Vector *else_children = vector_new(sizeof(StatementUnit *));
        {
            Vector *goto_toks = slice_until(&ts, T_SEMICOLON); // goto label;
            StatementUnit *g = make_goto_statement_unit(goto_toks, "label");
            vector_push_back(else_children, &g);
        }
        Vector *else_r = slice_exact(&ts, 1); // }

        vector_push_back(else_l, vector_get(else_r, 0));
        vector_free(else_r);

        StatementUnit *else_u = make_compound_statement_unit(else_l, else_children);

        StatementUnit *if_u = make_if_statement_unit(if_kw, cond_u, then_u, else_u);
        vector_push_back(root_children, &if_u);
    }

    // -------------------------
    // while(1) continue;
    // -------------------------
    {
        Vector *w_kw = slice_exact(&ts, 1); // while
        StatementUnit *cond = make_decl_or_expr_statement_unit(slice_until(&ts, T_RIGHT_PAREN));
        StatementUnit *body = make_continue_statement_unit(slice_until(&ts, T_SEMICOLON));
        StatementUnit *u = make_while_statement_unit(w_kw, cond, body);
        vector_push_back(root_children, &u);
    }

    // -------------------------
    // do { break; } while(0);
    // -------------------------
    {
        Vector *do_kw = slice_exact(&ts, 1); // do

        Vector *body_l = slice_exact(&ts, 1);
        Vector *body_children = vector_new(sizeof(StatementUnit *));
        {
            Vector *bk = slice_until(&ts, T_SEMICOLON);
            StatementUnit *bku = make_break_statement_unit(bk);
            vector_push_back(body_children, &bku);
        }
        Vector *body_r = slice_exact(&ts, 1);

        vector_push_back(body_l, vector_get(body_r, 0));
        vector_free(body_r);

        StatementUnit *body_u = make_compound_statement_unit(body_l, body_children);

        Vector *while_kw = slice_exact(&ts, 1);
        (void)while_kw; // while
        StatementUnit *cond = make_decl_or_expr_statement_unit(slice_until(&ts, T_RIGHT_PAREN));
        Vector *semi = slice_exact(&ts, 1);
        (void)semi;

        StatementUnit *u = make_do_while_statement_unit(do_kw, body_u, cond);
        vector_push_back(root_children, &u);
    }

    // -------------------------
    // for(;;);
    // -------------------------
    {
        Vector *for_kw = slice_exact(&ts, 1); // for
        Vector *lp = slice_exact(&ts, 1);
        (void)lp; // (

        StatementUnit *init = make_empty_statement_unit(slice_exact(&ts, 1)); // ;
        StatementUnit *cond = make_empty_statement_unit(slice_exact(&ts, 1)); // ;
        StatementUnit *step = NULL;

        Vector *rp = slice_exact(&ts, 1);
        (void)rp; // )

        StatementUnit *body = make_empty_statement_unit(slice_exact(&ts, 1));

        StatementUnit *u = make_for_statement_unit(for_kw, init, cond, step, body);
        vector_push_back(root_children, &u);
    }

    // -------------------------
    // switch (a) { case 1: a=2; default: ; }
    // -------------------------
    {
        Vector *sw_kw = slice_exact(&ts, 1); // switch
        StatementUnit *expr = make_decl_or_expr_statement_unit(slice_until(&ts, T_RIGHT_PAREN));

        Vector *body_l = slice_exact(&ts, 1);
        Vector *children = vector_new(sizeof(StatementUnit *));

        // case 1:
        Vector *case_kw = slice_exact(&ts, 1);
        StatementUnit *cv = make_decl_or_expr_statement_unit(slice_until(&ts, T_COLON));
        StatementUnit *cu = make_case_statement_unit(case_kw, cv);
        vector_push_back(children, &cu);

        // a=2;
        {
            Vector *toks = slice_until(&ts, T_SEMICOLON);
            StatementUnit *e = make_decl_or_expr_statement_unit(toks);
            vector_push_back(children, &e);
        }

        // default:
        {
            Vector *def_toks = slice_until(&ts, T_COLON);
            StatementUnit *d = make_default_statement_unit(def_toks, 0);
            vector_push_back(children, &d);
        }
        // ;
        {
            StatementUnit *e = make_empty_statement_unit(slice_exact(&ts, 1));
            vector_push_back(children, &e);
        }

        Vector *body_r = slice_exact(&ts, 1);
        vector_push_back(body_l, vector_get(body_r, 0));
        vector_free(body_r);

        StatementUnit *body = make_compound_statement_unit(body_l, children);

        StatementUnit *u = make_switch_statement_unit(sw_kw, expr, body);
        vector_push_back(root_children, &u);
    }

    // -------------------------
    // label: ;
    // -------------------------
    {
        Vector *label_toks = slice_until(&ts, T_COLON);
        StatementUnit *lu = make_label_statement_unit(label_toks, "label");
        vector_push_back(root_children, &lu);

        StatementUnit *e = make_empty_statement_unit(slice_until(&ts, T_SEMICOLON));
        vector_push_back(root_children, &e);
    }

    // -------------------------
    // root }
    // -------------------------
    Vector *root_end = slice_exact(&ts, 1);
    vector_push_back(root_tok, vector_get(root_end, 0));
    vector_free(root_end);

    // 构建根节点
    StatementUnit *root = make_compound_statement_unit(root_tok, root_children);

    print_statement_unit(root, 1);
    statement_unit_free(root);

    vector_free(ts.all_tokens);

    printf("=== Test OK ===\n");
}

int main()
{
    test_full();
    return 0;
}
