#include "decl_parser_impl/decl_specifier.h"
#include "decl_parser_impl/decl_specifier_impl/sue_types.h"
#include "decl_parser_impl/declarator.h"
#include "decl_parser_impl/declarator_impl/decl_initializer.h"
#include "vector.h"
#include "utils.h"
#include <stdlib.h>

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