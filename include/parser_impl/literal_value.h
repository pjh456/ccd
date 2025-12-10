#pragma once

typedef enum
{
    LIT_CHAR,
    LIT_INT,
    LIT_FLOAT,
    LIT_STRING
} LiteralType;

typedef struct
{
    LiteralType kind;
    union
    {
        int charv;

        struct
        {
            long long value;
            int unsig;
            int size;
        } intv; // size=1,2,4,8

        struct
        {
            double value;
            int size;
        } floatv; // 4 or 8 bytes

        struct
        {
            char *data;
            size_t len;
        } strv;
    };
} LiteralValue;