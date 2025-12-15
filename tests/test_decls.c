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

static Vector *vec_new_ptr(void)
{
    return vector_new(sizeof(void *));
}

static void vec_push(Vector *v, void *p)
{
    vector_push_back(v, &p);
}

static DeclInitializer *wrap_decl(Declarator *d)
{
    return make_decl_initializer(d, NULL);
}

static StatementUnit *fake_expr_stmt(const char *repr)
{
    Vector *tokens = vec_new_ptr();
    (void)repr;
    return make_decl_or_expr_statement_unit(tokens);
}

static void test_simple_decl(void)
{
    printf("\n=== test_simple_decl ===\n");

    DeclSpecifier *spec = make_decl_specifier(DBT_INT, DSUE_NONE, 0, 0, 0, 0);
    Declarator *id = make_identifier_declarator(strdup("a"));

    DeclInitializer *field_init = wrap_decl(id);
    Vector *vec = vec_new_ptr();
    vec_push(vec, field_init);

    DeclField *field = make_decl_field(spec, vec);
    // int a;

    Vector *fields = vec_new_ptr();
    vec_push(fields, field);
    DeclStructType *st = make_decl_struct_type("", fields); // anonymous struct for simple decl

    DeclSpecifier *struct_spec = make_decl_specifier(DBT_NONE, DSUE_STRUCT, DS_NONE, DFS_INLINE, DTQ_NONE, DTM_NONE);
    struct_spec->struct_type = st;
    Declarator *struct_id = make_identifier_declarator(strdup("b"));

    DeclInitializer *struct_init = wrap_decl(struct_id);
    Vector *structs = vector_new(sizeof(DeclInitializer *));
    vec_push(structs, struct_init);

    DeclUnit *unit = make_declaration_decl_unit(struct_spec, structs);
    print_decl_unit(unit);

    decl_unit_free(unit);
}

static void test_pointer_decl(void)
{
    printf("\n=== test_pointer_decl ===\n");

    DeclSpecifier *spec =
        make_decl_specifier(
            DBT_INT, DSUE_NONE, DS_STATIC, 0,
            DTQ_CONST, DTM_UNSIGNED | DTM_LONG);

    Declarator *id = make_identifier_declarator(strdup("p"));
    Declarator *ptr = make_pointer_declarator(id, DTQ_NONE);

    DeclInitializer *ptr_init = wrap_decl(ptr);
    Vector *decls = vec_new_ptr();
    vec_push(decls, ptr_init);

    DeclUnit *unit = make_declaration_decl_unit(spec, decls);
    print_decl_unit(unit);

    decl_unit_free(unit);
}

static void test_array_decl(void)
{
    printf("\n=== test_array_decl ===\n");

    DeclSpecifier *spec =
        make_decl_specifier(DBT_INT, DSUE_NONE, 0, 0, 0, 0);

    Declarator *id = make_identifier_declarator(strdup("arr"));
    Declarator *arr = make_array_declarator(id, 10);

    DeclInitializer *arr_init = wrap_decl(arr);
    Vector *decls = vec_new_ptr();
    vec_push(decls, arr_init);

    DeclUnit *unit = make_declaration_decl_unit(spec, decls);
    print_decl_unit(unit);

    decl_unit_free(unit);
}

static void test_function_decl(void)
{
    printf("\n=== test_function_decl ===\n");

    DeclSpecifier *spec =
        make_decl_specifier(DBT_INT, DSUE_NONE, 0, 0, 0, 0);

    DeclSpecifier *ps1 =
        make_decl_specifier(DBT_INT, DSUE_NONE, 0, 0, 0, 0);
    Declarator *pd1 = make_identifier_declarator(strdup("a"));
    DeclParam *p1 = make_decl_param(strdup("a"), ps1, pd1);

    DeclSpecifier *ps2 =
        make_decl_specifier(DBT_FLOAT, DSUE_NONE, 0, 0, 0, 0);
    Declarator *pd2 = make_identifier_declarator(strdup("b"));
    DeclParam *p2 = make_decl_param(strdup("b"), ps2, pd2);

    Vector *params = vec_new_ptr();
    vec_push(params, p1);
    vec_push(params, p2);

    Declarator *id = make_identifier_declarator(strdup("f"));
    Declarator *fn = make_function_declarator(id, params, 0);

    DeclInitializer *fn_init = wrap_decl(fn);
    Vector *decls = vec_new_ptr();
    vec_push(decls, fn_init);

    DeclUnit *unit = make_declaration_decl_unit(spec, decls);
    print_decl_unit(unit);

    decl_unit_free(unit);
}

static void test_initializer_decl(void)
{
    printf("\n=== test_initializer_decl ===\n");

    DeclSpecifier *spec =
        make_decl_specifier(DBT_INT, DSUE_NONE, 0, 0, 0, 0);

    Declarator *id = make_identifier_declarator(strdup("a"));
    StatementUnit *init_expr = fake_expr_stmt("1");
    DeclUnit *init_unit = make_expression_decl_unit(init_expr);

    DeclInitializer *init = make_decl_initializer(id, init_unit);

    Vector *decls = vec_new_ptr();
    vec_push(decls, init);

    DeclUnit *unit = make_declaration_decl_unit(spec, decls);
    print_decl_unit(unit);

    decl_unit_free(unit);
}

static void test_struct_decl(void)
{
    printf("\n=== test_struct_decl ===\n");

    DeclSpecifier *spec_x =
        make_decl_specifier(DBT_INT, DSUE_NONE, 0, 0, 0, 0);
    Declarator *dx = make_identifier_declarator(strdup("x"));

    DeclInitializer *fx_init = wrap_decl(dx);
    Vector *decls_x = vec_new_ptr();
    vec_push(decls_x, fx_init);
    DeclField *fx = make_decl_field(spec_x, decls_x);

    DeclSpecifier *spec_y =
        make_decl_specifier(DBT_FLOAT, DSUE_NONE, 0, 0, 0, 0);
    Declarator *dy = make_identifier_declarator(strdup("y"));

    DeclInitializer *dy_init = wrap_decl(dy);
    Vector *decls_y = vec_new_ptr();
    vec_push(decls_y, dy_init);
    DeclField *fy = make_decl_field(spec_y, decls_y);

    Vector *fields = vec_new_ptr();
    vec_push(fields, fx);
    vec_push(fields, fy);

    DeclStructType *st = make_decl_struct_type("S", fields);

    DeclSpecifier *spec =
        make_decl_specifier(DBT_NONE, DSUE_STRUCT, 0, 0, 0, 0);
    spec->struct_type = st;

    Vector *decls = vec_new_ptr();
    DeclUnit *unit = make_declaration_decl_unit(spec, decls);

    print_decl_unit(unit);
    decl_unit_free(unit);
}

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