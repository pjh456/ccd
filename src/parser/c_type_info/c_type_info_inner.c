#include "parser_impl/c_type_info.h"
#include "parser_impl/c_type_info_impl/c_type_info_impl.h"
#include "vector.h"
#include <stdlib.h>
#include <string.h>

Param *make_param_type(char *name, CTypeInfo *type)
{
    Param *param = malloc(sizeof(*param));
    memset(param, 0, sizeof(*param));

    if (name)
    {
        param->name = malloc(strlen(name) + 1);
        strcpy(param->name, name);
    }
    else
        param->name = NULL;
    param->type = type;
    return param;
}

Field *make_field_type(char *name, CTypeInfo *type, size_t offset)
{
    Field *field = malloc(sizeof(*field));
    memset(field, 0, sizeof(*field));

    if (name)
    {
        field->name = malloc(strlen(name) + 1);
        strcpy(field->name, name);
    }
    else
        field->name = NULL;
    field->type = type;
    field->offset = offset;
    return field;
}

EnumItem *make_enum_item_type(char *name, long long val)
{
    if (!name)
        return NULL;

    EnumItem *item = malloc(sizeof(*item));
    memset(item, 0, sizeof(*item));

    item->name = malloc(strlen(name) + 1);
    strcpy(item->name, name);
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