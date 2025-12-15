#pragma once

typedef struct DeclSpecifier DecpSpecifier;
typedef struct DeclField DeclField;
typedef struct DeclEnumItem DeclEnumItem;
typedef struct DeclStructType DeclStructType;
typedef struct DeclUnionType DeclUnionType;
typedef struct DeclEnumType DeclEnumType;

void print_decl_specifier_impl(DeclSpecifier *ds, int indent);

void print_decl_field_impl(DeclField *field, int indent);
void print_decl_enum_item_impl(DeclEnumItem *item, int indent);

void print_decl_struct_impl(DeclStructType *dst, int indent);
void print_decl_union_impl(DeclUnionType *dut, int indent);
void print_decl_enum_impl(DeclEnumType *det, int indent);