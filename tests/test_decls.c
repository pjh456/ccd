#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

/* ===== 引入你的头文件 ===== */
#include "decl_parser_impl/decl_specifier.h"
#include "decl_parser_impl/decl_specifier_impl/sue_types.h"
#include "decl_parser_impl/declarator.h"
#include "decl_parser_impl/declarator_impl/decl_param.h"

/* ========================= */

static void separator(const char *title)
{
    printf("\n================ %s ================\n", title);
}

int main(void)
{
    /* ========== 1. int a; ========== */
    separator("int a");

    DeclSpecifier *spec_int =
        make_decl_specifier(DBT_INT, DSUE_NONE, 0, 0, 0, 0);

    Declarator *decl_a =
        make_identifier_declarator("a");

    print_decl_specifier(spec_int);
    print_declarator(decl_a);

    decl_specifier_free(spec_int);
    declarator_free(decl_a);

    /* ========== 2. const int *p; ========== */
    separator("const int *p");

    DeclSpecifier *spec_const_int =
        make_decl_specifier(DBT_INT, DSUE_NONE, 0, 0, DTQ_CONST, 0);

    Declarator *decl_p =
        make_pointer_declarator(
            make_identifier_declarator("p"),
            0 /* pointer 自身无 qualifier */
        );

    print_decl_specifier(spec_const_int);
    print_declarator(decl_p);

    decl_specifier_free(spec_const_int);
    declarator_free(decl_p);

    /* ========== 3. double arr[10]; ========== */
    separator("double arr[10]");

    DeclSpecifier *spec_double =
        make_decl_specifier(DBT_DOUBLE, DSUE_NONE, 0, 0, 0, 0);

    Declarator *decl_arr =
        make_array_declarator(
            make_identifier_declarator("arr"),
            10);

    print_decl_specifier(spec_double);
    print_declarator(decl_arr);

    decl_specifier_free(spec_double);
    declarator_free(decl_arr);

    /* ========== 4. int f(int x, double y); ========== */
    separator("int f(int x, double y)");

    Vector *params = vector_new(sizeof(DeclParam *));

    DeclSpecifier *spec_param_int =
        make_decl_specifier(DBT_INT, DSUE_NONE, 0, 0, 0, 0);
    Declarator *decl_x =
        make_identifier_declarator("x");
    vector_push_back(
        params,
        &(DeclParam *){make_decl_param("x", spec_param_int, decl_x)});

    DeclSpecifier *spec_param_double =
        make_decl_specifier(DBT_DOUBLE, DSUE_NONE, 0, 0, 0, 0);
    Declarator *decl_y =
        make_identifier_declarator("y");
    vector_push_back(
        params,
        &(DeclParam *){make_decl_param("y", spec_param_double, decl_y)});

    DeclSpecifier *spec_func =
        make_decl_specifier(DBT_INT, DSUE_NONE, 0, 0, 0, 0);

    Declarator *decl_func =
        make_function_declarator(
            make_identifier_declarator("f"),
            params,
            0);

    print_decl_specifier(spec_func);
    print_declarator(decl_func);

    decl_specifier_free(spec_func);
    declarator_free(decl_func);

    /* ========== 5. int (*fp)(int); ========== */
    separator("int (*fp)(int)");

    Vector *fp_params = vector_new(sizeof(DeclParam *));

    DeclSpecifier *spec_fp_param =
        make_decl_specifier(DBT_INT, DSUE_NONE, 0, 0, 0, 0);
    Declarator *decl_fp_x =
        make_identifier_declarator("x");

    vector_push_back(fp_params,
                     &(DeclParam *){make_decl_param("x", spec_fp_param, decl_fp_x)});

    Declarator *fp_decl =
        make_function_declarator(
            make_pointer_declarator(
                make_identifier_declarator("fp"),
                0),
            fp_params,
            0);

    DeclSpecifier *spec_fp =
        make_decl_specifier(DBT_INT, DSUE_NONE, 0, 0, 0, 0);

    print_decl_specifier(spec_fp);
    print_declarator(fp_decl);

    decl_specifier_free(spec_fp);
    declarator_free(fp_decl);

    /* ========== 6. struct S s; ========== */
    separator("struct S s");

    DeclStructType *st = make_decl_struct_type("S", NULL);

    DeclSpecifier *spec_struct =
        make_decl_specifier(DBT_NONE, DSUE_STRUCT, 0, 0, 0, 0);
    spec_struct->struct_type = st;

    Declarator *decl_s =
        make_identifier_declarator("s");

    print_decl_specifier(spec_struct);
    print_declarator(decl_s);

    decl_specifier_free(spec_struct);
    declarator_free(decl_s);

    return 0;
}
