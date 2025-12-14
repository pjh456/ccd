#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"
#include "decl_parser_impl/decl_unit.h"
#include "decl_parser_impl/decl_specifier.h"
#include "decl_parser_impl/decl_specifier_impl/sue_types.h"
#include "decl_parser_impl/declarator.h"
#include "decl_parser_impl/declarator_impl/decl_initializer.h"
#include "decl_parser_impl/declarator_impl/decl_param.h"
#include "unit_scanner_impl/statement_unit.h"

Vector *vec_new_ptr(void)
{
    return vector_new(sizeof(void *));
}

void vec_push(Vector *v, void *p)
{
    vector_push_back(v, &p);
}

// -----------------------------
// 构造一些最小 StatementUnit
// -----------------------------

StatementUnit *fake_expr_stmt(const char *repr)
{
    // tokens 仅用于打印，不关心内容
    Vector *tokens = vec_new_ptr();
    (void)repr;
    return make_decl_or_expr_statement_unit(tokens);
}

// -----------------------------
// 测试 1：int a;
// -----------------------------

void test_simple_decl(void)
{
    printf("\n=== test_simple_decl ===\n");

    DeclSpecifier *spec = make_decl_specifier(
        DBT_INT,
        DSUE_NONE,
        0, 0, 0, 0);

    Declarator *id = make_identifier_declarator(strdup("a"));
    DeclInitializer *di = make_decl_initializer(id, NULL);

    Vector *decls = vec_new_ptr();
    vec_push(decls, di);

    DeclUnit *unit = make_declaration_decl_unit(spec, decls);
    print_decl_unit(unit);

    decl_unit_free(unit);
}

// -----------------------------
// 测试 2：static const unsigned long *p;
// -----------------------------

void test_pointer_decl(void)
{
    printf("\n=== test_pointer_decl ===\n");

    DeclSpecifier *spec = make_decl_specifier(
        DBT_INT,
        DSUE_NONE,
        DS_STATIC,
        0,
        DTQ_CONST,
        DTM_UNSIGNED | DTM_LONG);

    Declarator *id = make_identifier_declarator(strdup("p"));
    Declarator *ptr = make_pointer_declarator(id, DTQ_NONE);

    DeclInitializer *di = make_decl_initializer(ptr, NULL);

    Vector *decls = vec_new_ptr();
    vec_push(decls, di);

    DeclUnit *unit = make_declaration_decl_unit(spec, decls);
    print_decl_unit(unit);

    decl_unit_free(unit);
}

// -----------------------------
// 测试 3：int arr[10];
// -----------------------------

void test_array_decl(void)
{
    printf("\n=== test_array_decl ===\n");

    DeclSpecifier *spec = make_decl_specifier(
        DBT_INT,
        DSUE_NONE,
        0, 0, 0, 0);

    Declarator *id = make_identifier_declarator(strdup("arr"));
    Declarator *arr = make_array_declarator(id, 10);

    DeclInitializer *di = make_decl_initializer(arr, NULL);

    Vector *decls = vec_new_ptr();
    vec_push(decls, di);

    DeclUnit *unit = make_declaration_decl_unit(spec, decls);
    print_decl_unit(unit);

    decl_unit_free(unit);
}

// -----------------------------
// 测试 4：int f(int a, float b);
// -----------------------------

void test_function_decl(void)
{
    printf("\n=== test_function_decl ===\n");

    // 返回类型
    DeclSpecifier *spec = make_decl_specifier(
        DBT_INT,
        DSUE_NONE,
        0, 0, 0, 0);

    // 参数 a
    DeclSpecifier *ps1 = make_decl_specifier(DBT_INT, DSUE_NONE, 0, 0, 0, 0);
    Declarator *pd1 = make_identifier_declarator(strdup("a"));
    DeclParam *p1 = make_decl_param(strdup("a"), ps1, pd1);

    // 参数 b
    DeclSpecifier *ps2 = make_decl_specifier(DBT_FLOAT, DSUE_NONE, 0, 0, 0, 0);
    Declarator *pd2 = make_identifier_declarator(strdup("b"));
    DeclParam *p2 = make_decl_param(strdup("b"), ps2, pd2);

    Vector *params = vec_new_ptr();
    vec_push(params, p1);
    vec_push(params, p2);

    Declarator *id = make_identifier_declarator(strdup("f"));
    Declarator *fn = make_function_declarator(id, params, 0);

    DeclInitializer *di = make_decl_initializer(fn, NULL);

    Vector *decls = vec_new_ptr();
    vec_push(decls, di);

    DeclUnit *unit = make_declaration_decl_unit(spec, decls);
    print_decl_unit(unit);

    decl_unit_free(unit);
}

// -----------------------------
// 测试 5：int a = 1;
// -----------------------------

void test_initializer_decl(void)
{
    printf("\n=== test_initializer_decl ===\n");

    DeclSpecifier *spec = make_decl_specifier(
        DBT_INT,
        DSUE_NONE,
        0, 0, 0, 0);

    Declarator *id = make_identifier_declarator(strdup("a"));
    StatementUnit *init_expr = fake_expr_stmt("1");

    DeclUnit *init_unit = make_expression_decl_unit(init_expr);
    DeclInitializer *di = make_decl_initializer(id, init_unit);

    Vector *decls = vec_new_ptr();
    vec_push(decls, di);

    DeclUnit *unit = make_declaration_decl_unit(spec, decls);
    print_decl_unit(unit);

    decl_unit_free(unit);
}

// -----------------------------
// 测试 6：struct S { int x; float y; };
// -----------------------------

void test_struct_decl(void)
{
    printf("\n=== test_struct_decl ===\n");

    // field x
    DeclField *fx = calloc(1, sizeof(*fx));
    fx->name = strdup("x");
    fx->spec = make_decl_specifier(DBT_INT, DSUE_NONE, 0, 0, 0, 0);
    fx->decl = make_identifier_declarator(strdup("x"));

    // field y
    DeclField *fy = calloc(1, sizeof(*fy));
    fy->name = strdup("y");
    fy->spec = make_decl_specifier(DBT_FLOAT, DSUE_NONE, 0, 0, 0, 0);
    fy->decl = make_identifier_declarator(strdup("y"));

    Vector *fields = vec_new_ptr();
    vec_push(fields, fx);
    vec_push(fields, fy);

    DeclStructType *st = make_decl_struct_type("S", fields);

    DeclSpecifier *spec = make_decl_specifier(
        DBT_NONE,
        DSUE_STRUCT,
        0, 0, 0, 0);
    spec->struct_type = st;

    Vector *decls = vec_new_ptr();
    DeclUnit *unit = make_declaration_decl_unit(spec, decls);
    print_decl_unit(unit);

    decl_unit_free(unit);
}

// -----------------------------
// main
// -----------------------------

int main(void)
{
    test_simple_decl();
    test_pointer_decl();
    test_array_decl();
    test_function_decl();
    test_initializer_decl();
    test_struct_decl();
    return 0;
}
