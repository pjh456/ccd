#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser_impl/c_type_info.h"
#include "vector.h"

static void test_base_types()
{
    printf("==== BASE TYPES ====\n");
    CTypeInfo *t1 = make_void(CTS_NONE);
    CTypeInfo *t2 = make_char(CTS_NONE, CTQ_NONE, CTM_SIGNED);
    CTypeInfo *t3 = make_int(CTS_STATIC, CTQ_CONST, CTM_UNSIGNED);
    CTypeInfo *t4 = make_float(CTS_NONE, CTQ_NONE);
    CTypeInfo *t5 = make_double(CTS_EXTERN, CTQ_VOLATILE, CTM_LONGLONG);

    print_c_type_info(t1);
    print_c_type_info(t2);
    print_c_type_info(t3);
    print_c_type_info(t4);
    print_c_type_info(t5);

    c_type_info_free(t1);
    c_type_info_free(t2);
    c_type_info_free(t3);
    c_type_info_free(t4);
    c_type_info_free(t5);
}

static void test_pointer_and_array()
{
    printf("\n==== POINTER & ARRAY ====\n");
    CTypeInfo *ptr = make_pointer(make_int(CTS_NONE, CTQ_NONE, CTM_SIGNED));
    CTypeInfo *arr5 = make_array(make_int(CTS_NONE, CTQ_NONE, CTM_SIGNED), 5);
    CTypeInfo *arr10ptr = make_array(make_pointer(make_int(CTS_NONE, CTQ_NONE, CTM_SIGNED)), 10);

    print_c_type_info(ptr);
    print_c_type_info(arr5);
    print_c_type_info(arr10ptr);

    c_type_info_free(arr10ptr);
    c_type_info_free(arr5);
    c_type_info_free(ptr);
}

static void test_function_type()
{
    printf("\n==== FUNCTION TYPE ====\n");

    CTypeInfo *ret = make_int(CTS_NONE, CTQ_CONST, CTM_UNSIGNED);

    Vector *params = vector_new(sizeof(Param *));
    Param *p1 = make_param("p1", make_char(CTS_NONE, CTQ_NONE, CTM_SIGNED));
    Param *p2 = make_param(0, make_pointer(make_double(CTS_NONE, CTQ_NONE, 0)));

    vector_push_back(params, &p1);
    vector_push_back(params, &p2);

    CTypeInfo *fn = make_function(ret, params, 0);

    print_c_type_info(fn);

    c_type_info_free(fn);
}

static void test_struct()
{
    printf("\n==== STRUCT TYPE ====\n");

    Vector *fields = vector_new(sizeof(Field *));

    Field *a = make_field("a", make_int(CTS_NONE, CTQ_NONE, CTM_SIGNED), 0);
    Field *b = make_field("b", make_char(CTS_NONE, CTQ_NONE, CTM_SIGNED), 0);
    Field *c = make_field("c", make_double(CTS_NONE, CTQ_NONE, 0), 0);

    vector_push_back(fields, &a);
    vector_push_back(fields, &b);
    vector_push_back(fields, &c);

    CTypeInfo *s = make_struct(NULL);
    print_c_type_info(s);

    complete_struct(s, fields);
    print_c_type_info(s);

    c_type_info_free(s);
}

static void test_union()
{
    printf("\n==== UNION TYPE ====\n");

    Vector *fields = vector_new(sizeof(Field *));

    Field *a = make_field("u1", make_int(CTS_NONE, CTQ_NONE, CTM_SIGNED), 0);
    Field *b = make_field("u2", make_double(CTS_NONE, CTQ_NONE, 0), 0);

    vector_push_back(fields, &a);
    vector_push_back(fields, &b);

    CTypeInfo *u = make_union(NULL);
    print_c_type_info(u);

    complete_union(u, fields);
    print_c_type_info(u);

    c_type_info_free(u);
}

static void test_enum()
{
    printf("\n==== ENUM TYPE ====\n");

    Vector *items = vector_new(sizeof(EnumItem *));

    EnumItem *e1 = make_enum_item("ZERO", 0);
    EnumItem *e2 = make_enum_item("ONE", 1);
    EnumItem *e3 = make_enum_item("TEN", 10);

    vector_push_back(items, &e1);
    vector_push_back(items, &e2);
    vector_push_back(items, &e3);

    CTypeInfo *e = make_enum(NULL);
    print_c_type_info(e);

    complete_enum(e, items);
    print_c_type_info(e);

    c_type_info_free(e);
}

int main()
{
    test_base_types();
    test_pointer_and_array();
    test_function_type();
    test_struct();
    test_union();
    test_enum();

    return 0;
}
