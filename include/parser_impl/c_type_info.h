#pragma once

#include <stddef.h>

typedef enum
{
    CT_VOID,
    CT_CHAR,
    CT_INT,
    CT_FLOAT,
    CT_DOUBLE,

    CT_POINTER,
    CT_ARRAY,
    CT_FUNCTION,

    CT_STRUCT,
    CT_UNION,
    CT_ENUM,

    CT_UNKNOWN
} CType;

typedef enum
{
    CTS_NONE = 0,
    CTS_EXTERN = 1 << 0,
    CTS_STATIC = 1 << 1,
    CTS_REGISTER = 1 << 2
} CTypeStorage;

typedef enum
{
    FS_NONE = 0,
    FS_INLINE = 1 << 0,
    FS_NORETURN = 1 << 1
} CFuncSpecifier;

typedef enum
{
    CTQ_NONE = 0,
    CTQ_CONST = 1 << 0,
    CTQ_VOLATILE = 1 << 1,
    CTQ_RETRICT = 1 << 2,
} CTypeQualitifier;

typedef enum
{
    CTM_SIGNED = 0,
    CTM_UNSIGNED = 1 << 0,
    CTM_SHORT = 1 << 1,
    CTM_LONG = 1 << 2,
    CTM_LONGLONG = 1 << 3
} CTypeModifier;

typedef struct Vector Vector;
typedef struct CTypeInfo CTypeInfo;

typedef struct Param
{
    char *name;
    CTypeInfo *type;
} Param;

typedef struct Field
{
    char *name;
    CTypeInfo *type;
    size_t offset;
} Field;

typedef struct EnumItem
{
    char *name;
    long long value;
} EnumItem;

struct CTypeInfo
{
    CType type;

    unsigned storages;
    unsigned func_specifiers;
    unsigned qualifiers;
    unsigned modifiers;

    size_t size; // 以字节为单位
    size_t align;

    union
    {
        struct
        {
            CTypeInfo *base;
        } pointer;

        struct
        {
            CTypeInfo *base;
            size_t length;
        } array;

        struct
        {
            CTypeInfo *return_type;
            Vector *params; // Params
            int is_variadic;
        } func;

        // struct / union
        struct
        {
            int is_complete;
            Vector *fields; // Fields
        } record;

        // enum
        struct
        {
            int is_complete;
            Vector *items; // EnumItem
        } enum_type;
    };
};

CTypeInfo *make_void_type(unsigned storages);
CTypeInfo *make_char_type(unsigned storages, unsigned qualifiers, unsigned modifiers);
CTypeInfo *make_int_type(unsigned storages, unsigned qualifiers, unsigned modifiers);
CTypeInfo *make_float_type(unsigned storages, unsigned qualifiers);
CTypeInfo *make_double_type(unsigned storages, unsigned qualifiers, unsigned modifiers);

CTypeInfo *make_pointer_type(CTypeInfo *base);
CTypeInfo *make_array_type(CTypeInfo *base, size_t len);
CTypeInfo *make_function_type(CTypeInfo *ret, Vector *params, int is_var);

CTypeInfo *make_struct_type(Vector *fields);
void complete_struct_type(CTypeInfo *cti, Vector *fields);
CTypeInfo *make_union_type(Vector *fields);
void complete_union_type(CTypeInfo *cti, Vector *fields);
CTypeInfo *make_enum_type(Vector *items);
void complete_enum_type(CTypeInfo *cti, Vector *items);

Param *make_param_type(char *name, CTypeInfo *type);
Field *make_field_type(char *name, CTypeInfo *type, size_t offset);
EnumItem *make_enum_item_type(char *name, long long val);

CTypeInfo *make_unknown();

void c_type_info_free(CTypeInfo *cti);

char *c_type_name(CType ct);

void print_c_type_info(CTypeInfo *cti);