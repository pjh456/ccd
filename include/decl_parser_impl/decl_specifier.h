#pragma once

typedef struct Vector Vector;
typedef struct StructType StructType;
typedef struct UnionType UnionType;
typedef struct EnumType EnumType;
typedef struct DeclSpecifier DeclSpecifier;

typedef enum
{
    DBT_NONE = 0,
    DBT_VOID,
    DBT_CHAR,
    DBT_INT,
    DBT_FLOAT,
    DBT_DOUBLE,
} DeclBuiltinType;

typedef enum
{
    DSUE_NONE = 0,
    DSUE_STRUCT,
    DSUE_UNION,
    DSUE_ENUM,
} DeclSUE;

typedef enum
{
    DS_NONE = 0,
    DS_EXTERN = 1 << 0,
    DS_STATIC = 1 << 1,
    DS_REGISTER = 1 << 2,
    DS_TYPEDEF = 1 << 3,
} DeclStorage;

typedef enum
{
    DTQ_NONE = 0,
    DTQ_CONST = 1 << 0,
    DTQ_VOLATILE = 1 << 1,
    DTQ_RESTRICT = 1 << 2,
} DeclTypeQualifier;

typedef enum
{
    DTM_NONE = 0,
    DTM_UNSIGNED = 1 << 0,
    DTM_SHORT = 1 << 1,
    DTM_LONG = 1 << 2,
    DTM_LONGLONG = 1 << 3,
} DeclTypeModifier;

struct DeclSpecifier
{
    DeclBuiltinType builtin_type;

    DeclSUE sue_type;

    unsigned storages;
    unsigned func_specifiers;
    unsigned qualifiers;
    unsigned modifiers;

    union
    {
        StructType *struct_type;
        UnionType *union_type;
        EnumType *enum_type;
    };
};

DeclSpecifier *make_decl_specifier(
    DeclBuiltinType builtin_type,
    DeclSUE sue_type,
    unsigned storages,
    unsigned func_specs,
    unsigned qualifiers,
    unsigned modifiers);

void decl_specifier_free(DeclSpecifier *ds);