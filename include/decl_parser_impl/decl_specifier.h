#pragma once

typedef struct Vector Vector;
typedef struct DeclStructType DeclStructType;
typedef struct DeclUnionType DeclUnionType;
typedef struct DeclEnumType DeclEnumType;
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
    DFS_NONE = 0,
    DFS_INLINE = 1 << 0,
    DFS_NORETURN = 1 << 1
} DeclFuncSpecifieer;

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
    DTM_SIGNED = 1 << 0,
    DTM_UNSIGNED = 1 << 1,
    DTM_SHORT = 1 << 2,
    DTM_LONG = 1 << 3,
    DTM_LONGLONG = 1 << 4,
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
        DeclStructType *struct_type;
        DeclUnionType *union_type;
        DeclEnumType *enum_type;
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

const char *builtin_type_name(DeclBuiltinType t);

void print_decl_storages(unsigned s);
void print_decl_func_specifiers(unsigned fs);
void print_decl_qualifiers(unsigned q);
void print_decl_modifiers(unsigned mods);
void print_decl_specifier(DeclSpecifier *ds);