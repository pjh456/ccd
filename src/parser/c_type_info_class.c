#include "parser_impl/c_type_info.h"
#include "parser_impl/c_type_info_impl.h"
#include "vector.h"
#include <stdlib.h>

CTypeInfo *make_struct_type(Vector *fields)
{
    CTypeInfo *cti = malloc(sizeof(*cti));
    cti->type = CT_STRUCT;

    if (!fields)
    {
        cti->record.is_complete = 0;
        cti->record.fields = NULL;

        cti->size = 0;
        cti->align = 1;
        return cti;
    }

    complete_struct_type(cti, fields);
    return cti;
}

size_t align_up(size_t x, size_t align)
{
    return (x + align - 1) & ~(align - 1);
}

void complete_struct_type(CTypeInfo *cti, Vector *fields)
{
    if (!cti || !fields)
        return;
    if (cti->type != CT_STRUCT)
        return;

    cti->record.is_complete = 1;
    cti->record.fields = fields;
    // cti->record.fields = vector_new(fields->ele_size);
    // vector_resize(cti->record.fields, fields->size);

    size_t offset = 0, max_align = 1;
    for (size_t idx = 0; idx < fields->size; ++idx)
    {
        // Field *src = *((Field **)vector_get(fields, idx));
        // Field *dst = *((Field **)vector_get(cti->record.fields, idx));
        // *dst = *src;
        Field *dst = *((Field **)vector_get(cti->record.fields, idx));

        size_t falign = dst->type->align;
        size_t fsize = dst->type->size;

        if (falign > max_align)
            max_align = falign;

        offset = align_up(offset, falign);
        dst->offset = offset;

        offset += fsize;
    }

    offset = align_up(offset, max_align);

    cti->size = offset;
    cti->align = max_align;
}

CTypeInfo *make_union_type_type(Vector *fields)
{
    CTypeInfo *cti = malloc(sizeof(*cti));
    cti->type = CT_UNION;

    if (!fields)
    {
        cti->record.is_complete = 0;
        cti->record.fields = NULL;

        cti->size = 0;
        cti->align = 1;
        return cti;
    }

    complete_union_type(cti, fields);
    return cti;
}

void complete_union_type(CTypeInfo *cti, Vector *fields)
{
    if (!cti || !fields)
        return;
    if (cti->type != CT_UNION)
        return;

    cti->record.is_complete = 1;
    cti->record.fields = fields;
    // cti->record.fields = vector_new(fields->ele_size);
    // vector_resize(cti->record.fields, fields->size);

    size_t max_size = 0;
    size_t max_align = 1;

    for (size_t idx = 0; idx < fields->size; ++idx)
    {
        // Field *src = *((Field **)vector_get(fields, idx));
        // Field *dst = *((Field **)vector_get(cti->record.fields, idx));
        // *dst = *src;
        Field *dst = *((Field **)vector_get(cti->record.fields, idx));

        size_t falign = dst->type->align;
        size_t fsize = dst->type->size;

        dst->offset = 0;

        if (fsize > max_size)
            max_size = fsize;
        if (falign > max_align)
            max_align = falign;
    }

    max_size = align_up(max_size, max_align);

    cti->size = max_size;
    cti->align = max_align;
}

CTypeInfo *make_enum_type(Vector *items)
{
    CTypeInfo *cti = malloc(sizeof(*cti));
    cti->type = CT_ENUM;

    if (!items)
    {
        cti->enum_type.is_complete = 0;
        cti->enum_type.items = NULL;

        cti->size = cti->align = 0;
        return cti;
    }

    complete_enum_type(cti, items);
    return cti;
}

void complete_enum_type(CTypeInfo *cti, Vector *items)
{
    if (!cti || !items)
        return;
    if (cti->type != CT_ENUM)
        return;

    cti->enum_type.is_complete = 1;
    cti->enum_type.items = items;
    // cti->enum_type.items = vector_new(items->ele_size);
    // vector_resize(cti->enum_type.items, items->size);

    long long next_value = 0;
    for (size_t i = 0; i < items->size; ++i)
    {
        // EnumItem *src = *((EnumItem **)vector_get(items, i));
        // EnumItem *dst = *((EnumItem **)vector_get(cti->enum_type.items, i));
        // *dst = *src;
        EnumItem *dst = *((EnumItem **)vector_get(cti->enum_type.items, i));

        if (dst->value == LLONG_MIN)
            dst->value = next_value;

        next_value = dst->value + 1;
    }

    cti->size = sizeof(int);
    cti->align = sizeof(int);
}

void c_struct_type_info_free(CTypeInfo *cti)
{
    if (!cti || cti->type != CT_STRUCT)
        return;
    if (cti->record.fields)
    {
        for (size_t idx = 0; idx < cti->record.fields->size; ++idx)
            c_field_type_info_free(*((Field **)vector_get(cti->record.fields, idx)));
        vector_free(cti->record.fields);
    }
    free(cti);
}

void c_union_type_info_free(CTypeInfo *cti)
{
    if (!cti || cti->type != CT_UNION)
        return;
    if (cti->record.fields)
    {
        for (size_t idx = 0; idx < cti->record.fields->size; ++idx)
            c_field_type_info_free(*((Field **)vector_get(cti->record.fields, idx)));
        vector_free(cti->record.fields);
    }
    free(cti);
}

void c_enum_type_info_free(CTypeInfo *cti)
{
    if (!cti || cti->type != CT_ENUM)
        return;
    if (cti->enum_type.items)
    {
        for (size_t idx = 0; idx < cti->enum_type.items->size; ++idx)
            c_enum_item_type_info_free(*((EnumItem **)vector_get(cti->enum_type.items, idx)));
        vector_free(cti->record.fields);
    }
    free(cti);
}
