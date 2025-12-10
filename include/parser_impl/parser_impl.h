#pragma once

#include "c_type_info.h"
#include "literal_value.h"
#include "expression.h"
#include "statement.h"

typedef struct Parser Parser;

Declaration *parse_declaration_or_function(Parser *p);

Declaration *parse_function_definition(Parser *p, CTypeInfo *typeinfo, char *name);
Declaration *parse_variable_declaration(Parser *p, CTypeInfo *typeinfo, char *name);

Declaration *parse_typedef(Parser *p, CTypeInfo *typeinfo, char *name);
Declaration *parse_struct_or_union(Parser *p);
Declaration *parse_enum(Parser *p);

// 类型说明符：int / char / long / unsigned / struct / typedef name 等
CTypeInfo *parse_declspec(Parser *p);
// 声明符：指针、数组、函数指针等
CTypeInfo *parse_declarator(Parser *p, char **out_name);
// 用于处理 arrays, function(params) 等后缀
CTypeInfo *parse_direct_declarator(Parser *p, char **out_name);
// struct 成员解析
// Field *parse_struct_members(Parser *p);

// 参数列表解析（函数声明）
// Param **parse_parameter_list(Parser *p);
// Param *parse_parameter_declaration(Parser *p);
// 类型后缀：[] ()
CTypeInfo *parse_type_suffix(Parser *p, CTypeInfo *base);