#pragma once

#include <stddef.h>

typedef enum
{
    CT_VOID,
    CT_CHAR,
    CT_SHORT,
    CT_INT,
    CT_LONG,
    CT_LONGLONG,
    CT_FLOAT,
    CT_DOUBLE,

    CT_POINTER,
    CT_ARRAY,
    CT_FUNCTION,

    CT_STRUCT,
    CT_UNION,
    CT_ENUM
} CType;

typedef struct CTypeInfo CTypeInfo;

struct CTypeInfo
{
    CType kind;

    size_t size; // 以字节为单位
    size_t align;

    union
    {
        // pointer
        struct
        {
            CTypeInfo *base;
        } pointer;

        // array
        struct
        {
            CTypeInfo *base;
            size_t length;
        } array;

        // function
        struct
        {
            CTypeInfo *return_type;
            CTypeInfo **params;
            int param_count;
            int is_variadic;
        } func;

        // struct / union
        struct
        {
            struct Field
            {
                char *name;
                CTypeInfo *type;
                int offset;
            } *fields;
            int field_count;

            // 用于标识完整/不完整类型
            int is_complete;
        } record;

        // enum
        struct
        {
            // enum 本质是 int，但保留符号表也有用
            struct EnumItem
            {
                char *name;
                long long value;
            } *items;
            int item_count;
        } enum_type;
    };
};
