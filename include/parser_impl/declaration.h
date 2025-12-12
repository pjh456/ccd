#pragma once

typedef struct Vector Vector;
typedef struct CTypeInfo CTypeInfo;
typedef struct Expression Expression;
typedef struct DeclarationSpecifier DeclarationSpecifier;
typedef struct Initializer Initializer;
typedef struct Declarator Declarator;
typedef struct Declaration Declaration;

struct DeclarationSpecifier
{
    CTypeInfo *base_type;
    int is_typedef;
    int is_extern;
    int is_static;
    int is_const;
    int is_volatile;
};

struct Initializer
{
    int is_list;      // 0: expr, 1: list
    Expression *expr; // if expr-init
    Vector *list;     // vector<Initializer*>
    char *designator; // .field 或 [index]
};

struct Declarator
{
    char *name;
    CTypeInfo *type;   // 完整类型（specifier + declarator 合并后的）
    Initializer *init; // 初始化器（可能是表达式或列表）
};

struct Declaration
{
    CTypeInfo *type_info; // 包含指针、数组等完整类型
    char *name;           // 声明的名字
    Expression *init;     // 初始化器，可为 NULL
};

// Declaration *make_declaration(CTypeInfo *cti, char *name, Expression *expr);

// void declaration_free(Declaration *decl);