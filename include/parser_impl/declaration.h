#pragma once

#include "c_type_info.h"
#include "expression.h"

typedef struct Declaration Declaration;

struct Declaration
{
    CTypeInfo *typeinfo; // 包含指针、数组等完整类型
    char *name;          // 声明的名字
    Expression *init;    // 初始化器，可为 NULL
};