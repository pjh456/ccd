#include "decl_parser_impl/decl_specifier.h"
#include "decl_parser_impl/decl_specifier_impl/sue_types.h"
#include "decl_parser_impl/decl_specifier_impl/decl_specifier_impl.h"
#include "decl_parser_impl/declarator.h"
#include "decl_parser_impl/declarator_impl/decl_initializer.h"
#include "decl_parser_impl/declarator_impl/declarator_impl.h"
#include "vector.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>

DeclStructType *make_decl_struct_type(const char *name, Vector *fields)
{
    DeclStructType *dst = malloc(sizeof(*dst));

    dst->name = str_clone(name);
    dst->is_complete = (fields != 0);
    if (fields)
        complete_decl_struct_type(dst, fields);

    return dst;
}

void decl_struct_type_free(DeclStructType *dst)
{
    if (!dst)
        return;
    if (dst->name)
        free(dst->name);
    if (dst->fields)
    {
        for (size_t idx = 0; idx < dst->fields->size; ++idx)
            decl_field_free(*((DeclField **)vector_get(dst->fields, idx)));
        vector_free(dst->fields);
    }
    free(dst);
}

void complete_decl_struct_type(DeclStructType *dst, Vector *fields)
{
    if (!dst || !fields)
        return;

    dst->fields = fields;
    dst->is_complete = 1;
}

DeclUnionType *make_decl_union_type(const char *name, Vector *fields)
{
    DeclUnionType *dut = malloc(sizeof(*dut));

    dut->name = str_clone(name);
    dut->is_complete = (fields != 0);
    if (fields)
        complete_decl_union_type(dut, fields);

    return dut;
}

void complete_decl_union_type(DeclUnionType *dut, Vector *fields)
{
    if (!dut || !fields)
        return;

    dut->fields = fields;
    dut->is_complete = 1;
}

void decl_union_type_free(DeclUnionType *dut)
{
    if (!dut)
        return;
    if (dut->name)
        free(dut->name);
    if (dut->fields)
    {
        for (size_t idx = 0; idx < dut->fields->size; ++idx)
            decl_field_free(*((DeclField **)vector_get(dut->fields, idx)));
        vector_free(dut->fields);
    }
    free(dut);
}

DeclEnumType *make_decl_enum_type(const char *name, Vector *items)
{
    DeclEnumType *det = malloc(sizeof(*det));

    det->name = str_clone(name);
    det->is_complete = (items != 0);
    if (items)
        complete_decl_enum_type(det, items);

    return det;
}

void complete_decl_enum_type(DeclEnumType *det, Vector *items)
{
    if (!det || !items)
        return;

    det->items = items;
    det->is_complete = 1;
}

void decl_enum_type_free(DeclEnumType *det)
{
    if (!det)
        return;
    if (det->name)
        free(det->name);
    if (det->items)
    {
        for (size_t idx = 0; idx < det->items->size; ++idx)
            decl_enum_item_free(*((DeclEnumItem **)vector_get(det->items, idx)));
        vector_free(det->items);
    }
    free(det);
}

DeclField *make_decl_field(DeclSpecifier *spec, Vector *decls)
{
    if (!spec || !decls)
        return NULL;
    DeclField *field = calloc(1, sizeof(*field));

    field->spec = spec;
    field->decls = decls;

    return field;
}

void decl_field_free(DeclField *df)
{
    if (!df)
        return;
    decl_specifier_free(df->spec);
    if (df->decls)
    {
        for (size_t idx = 0; idx < df->decls->size; ++idx)
            decl_initializer_free(*((DeclInitializer **)vector_get(df->decls, idx)));
        vector_free(df->decls);
    }
    free(df);
}

DeclEnumItem *make_decl_enum_item(const char *name, int has_value, long long value)
{
    DeclEnumItem *item = calloc(1, sizeof(*item));

    item->name = str_clone(name);
    item->has_value = has_value;
    if (has_value)
        item->value = value;

    return item;
}

void decl_enum_item_free(DeclEnumItem *dei)
{
    if (!dei)
        return;
    if (dei->name)
        free(dei->name);
    free(dei);
}

void print_decl_field_impl(DeclField *field, int indent)
{
    if (!field)
    {
        print_indent(indent);
        printf("<null DeclField>\n");
        return;
    }

    print_indent(indent);
    printf("Field:\n");

    print_indent(indent + 2);
    printf("Specifier:\n");
    print_decl_specifier_impl(field->spec, indent + 4);

    print_indent(indent + 2);
    printf("Declarators (%zu):\n",
           field->decls ? field->decls->size : 0);

    if (field->decls)
    {
        for (size_t i = 0; i < field->decls->size; ++i)
        {
            DeclInitializer *init =
                *(DeclInitializer **)vector_get(field->decls, i);

            print_indent(indent + 4);
            printf("Declarator #%zu:\n", i);
            print_decl_initializer_impl(init, indent + 6);
        }
    }
}

void print_decl_enum_item_impl(DeclEnumItem *item, int indent)
{
    if (!item)
    {
        print_indent(indent);
        printf("<null EnumItem>\n");
        return;
    }

    print_indent(indent);
    printf("EnumItem: %s", item->name ? item->name : "<anonymous>");

    if (item->has_value)
        printf(" = %lld", item->value);

    printf("\n");
}

void print_decl_struct_impl(DeclStructType *dst, int indent)
{
    if (!dst)
    {
        print_indent(indent + 2);
        printf("<null StructType>\n");
        return;
    }

    print_indent(indent + 2);
    printf("Struct %s",
           dst->name ? dst->name : "<anonymous>");

    printf(dst->is_complete ? " (complete)\n" : " (incomplete)\n");

    if (!dst->is_complete || !dst->fields)
        return;

    print_indent(indent + 2);
    printf("Fields (%zu):\n", dst->fields->size);

    for (size_t i = 0; i < dst->fields->size; ++i)
    {
        DeclField *field =
            *(DeclField **)vector_get(dst->fields, i);

        print_indent(indent + 4);
        printf("Field #%zu:\n", i);
        print_decl_field_impl(field, indent + 6);
    }
}

void print_decl_union_impl(DeclUnionType *dut, int indent)
{
    if (!dut)
    {
        print_indent(indent);
        printf("<null UnionType>\n");
        return;
    }

    print_indent(indent);
    printf("Union %s",
           dut->name ? dut->name : "<anonymous>");

    printf(dut->is_complete ? " (complete)\n" : " (incomplete)\n");

    if (!dut->is_complete || !dut->fields)
        return;

    print_indent(indent + 2);
    printf("Fields (%zu):\n", dut->fields->size);

    for (size_t i = 0; i < dut->fields->size; ++i)
    {
        DeclField *field =
            *(DeclField **)vector_get(dut->fields, i);

        print_indent(indent + 4);
        printf("Field #%zu:\n", i);
        print_decl_field_impl(field, indent + 6);
    }
}

void print_decl_enum_impl(DeclEnumType *det, int indent)
{
    if (!det)
    {
        print_indent(indent);
        printf("<null EnumType>\n");
        return;
    }

    print_indent(indent);
    printf("Enum %s",
           det->name ? det->name : "<anonymous>");

    printf(det->is_complete ? " (complete)\n" : " (incomplete)\n");

    if (!det->is_complete || !det->items)
        return;

    print_indent(indent + 2);
    printf("Items (%zu):\n", det->items->size);

    for (size_t i = 0; i < det->items->size; ++i)
    {
        DeclEnumItem *item =
            *(DeclEnumItem **)vector_get(det->items, i);

        print_decl_enum_item_impl(item, indent + 4);
    }
}