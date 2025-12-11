#pragma once

typedef struct CTypeInfo CTypeInfo;

void c_void_type_info_free(CTypeInfo *cti);
void c_char_type_info_free(CTypeInfo *cti);
void c_int_type_info_free(CTypeInfo *cti);
void c_float_type_info_free(CTypeInfo *cti);
void c_double_type_info_free(CTypeInfo *cti);

void c_pointer_type_info_free(CTypeInfo *cti);
void c_array_type_info_free(CTypeInfo *cti);

void c_function_type_info_free(CTypeInfo *cti);

void c_struct_type_info_free(CTypeInfo *cti);
void c_union_type_info_free(CTypeInfo *cti);
void c_enum_type_info_free(CTypeInfo *cti);

void c_param_type_info_free(Param *param);
void c_field_type_info_free(Field *field);
void c_enum_item_type_info_free(EnumItem *item);

void print_c_type_info_impl(CTypeInfo *cti, int indent);