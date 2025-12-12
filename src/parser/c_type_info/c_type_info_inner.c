#include "parser_impl/c_type_info.h"
#include "parser_impl/c_type_info_impl/c_type_info_impl.h"
#include "vector.h"
#include "utils.h"
#include <stdlib.h>

Param *make_param_type(char *name, CTypeInfo *type)
{
    Param *param = calloc(1, sizeof(*param));

    param->name = str_clone(name);
    param->type = type;
    return param;
}

Field *make_field_type(char *name, CTypeInfo *type, size_t offset)
{
    Field *field = calloc(1, sizeof(*field));

    field->name = str_clone(name);
    field->type = type;
    field->offset = offset;
    return field;
}

EnumItem *make_enum_item_type(char *name, long long val)
{
    if (!name)
        return NULL;

    EnumItem *item = calloc(1, sizeof(*item));

    item->name = str_clone(name);
    item->value = val;
    return item;
}

void c_param_type_info_free(Param *param)
{
    if (!param)
        return;
    if (param->name)
        free(param->name);
    c_type_info_free(param->type);
    free(param);
}

void c_field_type_info_free(Field *field)
{
    if (!field)
        return;
    if (field->name)
        free(field->name);
    c_type_info_free(field->type);
    free(field);
}

void c_enum_item_type_info_free(EnumItem *item)
{
    if (!item)
        return;
    if (item->name)
        free(item->name);
    free(item);
}