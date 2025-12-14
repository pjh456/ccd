#pragma once

typedef struct Vector Vector;
typedef struct DeclField DeclField;
typedef struct DeclEnumItem DeclEnumItem;
typedef struct DeclStructType DeclStructType;
typedef struct DeclUnionType DeclUnionType;
typedef struct DeclEnumType DeclEnumType;
typedef struct DeclSpecifier DeclSpecifier;
typedef struct Declarator Declarator;

struct DeclField
{
    char *name;
    DeclSpecifier *spec;
    Declarator *decl;
};

struct DeclEnumItem
{
    char *name;
    int has_value;
    long long value;
};

struct DeclStructType
{
    char *name;
    Vector *fields; // DeclFields
    int is_complete;
};

struct DeclUnionType
{
    char *name;
    Vector *fields; // DeclFields
    int is_complete;
};

struct DeclEnumType
{
    char *name;
    Vector *items; // DeclEnumItems
    int is_complete;
};

DeclStructType *make_decl_struct_type(const char *name, Vector *fields);
void complete_decl_struct_type(DeclStructType *dst, Vector *fields);
void decl_struct_type_free(DeclStructType *dst);

DeclUnionType *make_decl_union_type(const char *name, Vector *fields);
void complete_decl_union_type(DeclUnionType *dut, Vector *fields);
void decl_union_type_free(DeclUnionType *dut);

DeclEnumType *make_decl_enum_type(const char *name, Vector *items);
void complete_decl_enum_type(DeclEnumType *det, Vector *items);
void decl_enum_type_free(DeclEnumType *det);

void decl_field_free(DeclField *df);
void decl_enum_item_free(DeclEnumItem *dei);