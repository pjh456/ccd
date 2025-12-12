#include "parser_impl/c_type_info.h"
#include "parser_impl/c_type_info_impl/c_type_info_impl.h"
#include <stdlib.h>
#include <string.h>

CTypeInfo *make_pointer_type(CTypeInfo *base)
{
    CTypeInfo *cti = malloc(sizeof(*cti));
    memset(cti, 0, sizeof(*cti));

    cti->type = CT_POINTER;
    cti->pointer.base = base;
    cti->size = cti->align = sizeof(void *);
    return cti;
}

CTypeInfo *make_array_type(CTypeInfo *base, size_t len)
{
    CTypeInfo *cti = malloc(sizeof(*cti));
    memset(cti, 0, sizeof(*cti));

    cti->type = CT_ARRAY;
    cti->array.base = base;
    cti->array.length = len;
    cti->size = cti->align = sizeof(void *);
    return cti;
}

void c_pointer_type_info_free(CTypeInfo *cti)
{
    if (!cti || cti->type != CT_POINTER)
        return;
    c_type_info_free(cti->pointer.base);
    free(cti);
}

void c_array_type_info_free(CTypeInfo *cti)
{
    if (!cti || cti->type != CT_ARRAY)
        return;
    c_type_info_free(cti->array.base);
    free(cti);
}