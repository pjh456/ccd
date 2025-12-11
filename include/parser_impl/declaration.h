#pragma once

typedef struct CTypeInfo CTypeInfo;
typedef struct Expression Expression;
typedef struct Declaration Declaration;

struct Declaration
{
    CTypeInfo *type_info; // 包含指针、数组等完整类型
    char *name;           // 声明的名字
    Expression *init;     // 初始化器，可为 NULL
};

Declaration *make_declaration(CTypeInfo *cti, char *name, Expression *expr);

void declaration_free(Declaration *decl);