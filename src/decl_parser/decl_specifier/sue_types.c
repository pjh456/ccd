#include "decl_parser_impl/decl_specifier_impl/sue_types.h"
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