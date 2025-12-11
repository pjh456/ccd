#include "parser_impl/c_type_info.h"
#include "parser_impl/c_type_info_impl.h"
#include <stdlib.h>
#include <string.h>

CTypeInfo *make_void_type(unsigned storages)
{
    CTypeInfo *cti = malloc(sizeof(*cti));
    cti->type = CT_VOID;
    cti->storages = storages;
    cti->size = 0, cti->align = 1;
    return cti;
}

CTypeInfo *make_char_type(unsigned storages, unsigned qualifiers, unsigned modifiers)
{
    CTypeInfo *cti = malloc(sizeof(*cti));
    memset(cti, 0, sizeof(*cti));

    cti->type = CT_CHAR;
    cti->storages = storages;
    cti->qualifiers = qualifiers;
    cti->modifiers = modifiers;

    int has_unsigned = (modifiers & CTM_UNSIGNED) != 0;

    if (has_unsigned)
        cti->size = cti->align = sizeof(unsigned char);
    else
        cti->size = cti->align = sizeof(signed char);

    return cti;
}

CTypeInfo *make_int_type(unsigned storages, unsigned qualifiers, unsigned modifiers)
{
    CTypeInfo *cti = malloc(sizeof(*cti));
    memset(cti, 0, sizeof(*cti));

    cti->type = CT_INT;
    cti->storages = storages;
    cti->qualifiers = qualifiers;
    cti->modifiers = modifiers;

    int has_longlong = (modifiers & CTM_LONGLONG) != 0;
    int has_long = (modifiers & CTM_LONG) != 0;
    int has_short = (modifiers & CTM_SHORT) != 0;
    int has_unsigned = (modifiers & CTM_UNSIGNED) != 0;

    if (has_longlong)
    {
        if (has_unsigned)
            cti->size = cti->align = sizeof(unsigned long long int);
        else
            cti->size = cti->align = sizeof(signed long long int);
    }
    else if (has_long)
    {
        if (has_unsigned)
            cti->size = cti->align = sizeof(unsigned long int);
        else
            cti->size = cti->align = sizeof(signed long int);
    }
    else if (has_short)
    {
        if (has_unsigned)
            cti->size = cti->align = sizeof(unsigned short int);
        else
            cti->size = cti->align = sizeof(signed short int);
    }
    else
    {
        if (has_unsigned)
            cti->size = cti->align = sizeof(unsigned int);
        else
            cti->size = cti->align = sizeof(signed int);
    }

    return cti;
}

CTypeInfo *make_float_type(unsigned storages, unsigned qualifiers)
{
    CTypeInfo *cti = malloc(sizeof(*cti));
    memset(cti, 0, sizeof(*cti));

    cti->type = CT_FLOAT;
    cti->storages = storages;
    cti->qualifiers = qualifiers;
    cti->size = cti->align = sizeof(float);
    return cti;
}

CTypeInfo *make_double_type(unsigned storages, unsigned qualifiers, unsigned modifiers)
{
    CTypeInfo *cti = malloc(sizeof(*cti));
    memset(cti, 0, sizeof(*cti));

    cti->type = CT_DOUBLE;
    cti->storages = storages;
    cti->qualifiers = qualifiers;
    cti->modifiers = modifiers;

    int has_long = (modifiers & CTM_LONG) != 0;

    if (has_long)
        cti->size = cti->align = sizeof(long double);
    else
        cti->size = cti->align = sizeof(double);

    return cti;
}

void c_void_type_info_free(CTypeInfo *cti)
{
    if (!cti || cti->type != CT_VOID)
        return;
    free(cti);
}

void c_char_type_info_free(CTypeInfo *cti)
{
    if (!cti || cti->type != CT_CHAR)
        return;
    free(cti);
}

void c_int_type_info_free(CTypeInfo *cti)
{
    if (!cti || cti->type != CT_INT)
        return;
    free(cti);
}

void c_float_type_info_free(CTypeInfo *cti)
{
    if (!cti || cti->type != CT_FLOAT)
        return;
    free(cti);
}

void c_double_type_info_free(CTypeInfo *cti)
{
    if (!cti || cti->type != CT_DOUBLE)
        return;
    free(cti);
}
