# 14. C 语言声明的奥秘：类型与变量的分离

**本章目标**：

1.  **痛点直击**：为什么 `int *a[5]` 和 `int (*a)[5]` 含义完全不同？
2.  **核心概念**：彻底理解 **Type Specifier (类型说明符)** 与 **Declarator (声明符)** 的区别。
3.  **数据结构**：解读 `DeclSpecifier` 和 `Declarator` 结构体设计，为解析器做准备。

---

## 1. 最难懂的 C 语言语法：声明

如果我问你：`int a;` 是什么意思？
你会说：定义了一个整数变量 `a`。

那 `int *a[10];` 呢？
你可能会犹豫：是一个指针数组？还是一个指向数组的指针？

C 语言的声明语法有一句著名的格言：**"Declaration mimics use" (声明模仿使用)**。

*   声明 `int *p;` 意味着：表达式 `*p` 的结果是 `int`。
*   声明 `int a[10];` 意味着：表达式 `a[i]` 的结果是 `int`。
*   声明 `int f();` 意味着：表达式 `f()` 的结果是 `int`。

为了让计算机（查重器）理解这些复杂的声明，我们需要把声明拆解为两个核心部分：

1.  **DeclSpecifier (类型说明符)**：`int`, `const char`, `static unsigned long` —— 描述**基础性质**。
2.  **Declarator (声明符)**：`*p`, `a[10]`, `(*fp)(int)` —— 描述**名字及其衍生性质**（指针/数组/函数）。

---

## 2. 第一部分：DeclSpecifier (类型说明符)

这是声明的基石。它告诉我们这个变量“本质上”是什么。

看看 `include/decl_specifier.h` 的设计：

```c
struct DeclSpecifier
{
    DeclBuiltinType builtin_type; // int, char, void...
    
    // 存储类 (static, extern...)
    unsigned storages;
    
    // 类型限定符 (const, volatile...)
    unsigned qualifiers;
    
    // 结构体/联合体/枚举 (struct A { ... })
    DeclSUE sue_type;
    union {
        DeclStructType *struct_type;
        DeclUnionType *union_type;
        DeclEnumType *enum_type;
    };
};
```

例子：`static const unsigned int a;`
*   `builtin_type`: `DBT_INT`
*   `storages`: `DS_STATIC`
*   `qualifiers`: `DTQ_CONST`
*   `modifiers`: `DTM_UNSIGNED`

这一部分相对简单，它是声明的“左半边”。

---

## 3. 第二部分：Declarator (声明符)

这是最烧脑的部分。它是声明的“右半边”，负责定义变量名以及它是指针、数组还是函数。

看看 `include/declarator.h` 的递归设计：

```c
typedef enum {
    DRT_IDENT,    // 叶子：变量名 (a)
    DRT_POINTER,  // 包装器：指针 (*a)
    DRT_ARRAY,    // 包装器：数组 (a[])
    DRT_FUNCTION, // 包装器：函数 (a())
    DRT_GROUP,    // 包装器：括号 ((a))
} DeclaratorType;

struct Declarator
{
    DeclaratorType type;
    char *name; // 如果是叶子节点，这里存 "a"

    union
    {
        // 指针：指向另一个 Declarator
        struct {
            Declarator *inner;
            unsigned qualifiers; // int * const p;
        } pointer;

        // 数组：包含另一个 Declarator + 长度
        struct {
            Declarator *inner;
            DeclUnit *length; 
        } array;

        // 函数：包含另一个 Declarator + 参数列表
        struct {
            Declarator *inner;
            Vector *params;
        } function;
        
        // 分组：( ... )
        struct {
            Declarator *inner;
        } group;
    };
};
```

**为什么是递归的？**

让我们像剥洋葱一样解析 `int *(*a)[5];`：

1.  最外层是 `int` (**DeclSpecifier**)。
2.  剩下的是 `*(*a)[5]` (**Declarator**)。
3.  最外层操作符是 `*` (第一个星号)，所以它是一个 **Pointer Declarator**。
    *   它指向里面：`(*a)[5]`。
4.  `(*a)[5]` 最外层是 `[]` (数组优先于指针吗？不，这里有括号)。
    *   等等，因为有括号 `(*a)`，所以我们先看括号里。
    *   括号里是 `*a`。
5.  `*a` 是一个 **Pointer Declarator**。
    *   它指向里面：`a`。
6.  `a` 是 **Identifier Declarator** (终点)。

反过来理解：
*   `a` 是一个指针。
*   它指向一个数组 `[5]`。
*   数组的元素是 `int *` (指针)。

所以 `int *(*a)[5]` 是：**一个指向（包含5个int指针的数组）的指针**。

---

## 4. 组合：DeclUnit

最后，我们将两者结合，形成一个完整的声明单元。

看看 `include/decl_unit.h`：

```c
struct DeclUnit
{
    DeclUnitType type; // DUT_DECLARATION
    union
    {
        struct
        {
            DeclSpecifier *spec; // int
            Vector *decls;       // a, *b = NULL, c[10]; (可以有多个)
        } decl;
        // ...
    };
};
```

注意 `Vector *decls`。因为 C 语言允许一行声明多个变量：
```c
int a, *b, c[10];
```
这里：
*   `spec` 是 `int`（大家共用）。
*   `decls` 列表里有三个 `DeclInitializer`：
    1.  `a` (Identifier)
    2.  `*b` (Pointer -> Identifier)
    3.  `c[10]` (Array -> Identifier)

---

## 5. 符号表 (Scope & Symbol)

解析声明不仅仅是为了看，更是为了记。
我们需要一个 **符号表 (Symbol Table)** 来记录：
*   变量 `a` 是什么类型？
*   函数 `main` 返回什么？
*   结构体 `Student` 有哪些字段？

看看 `include/scope.h` 和 `include/symbol.h`：

```c
struct Scope {
    Scope *parent;    // 父作用域 (实现类似 { { ... } } 的嵌套查找)
    HashMap *idents;  // 变量名表
    HashMap *tags;    // struct/union/enum 标签表
};

struct Symbol {
    char *name;
    SymbolType type;     // 变量? 函数? 类型定义?
    DeclSpecifier *spec; // 类型信息
    Declarator *decl;    // 声明信息
};
```

当我们解析到 `int a;` 时，我们会创建一个 `Symbol`，把它塞进当前的 `Scope` 里。
以后如果在代码里看到 `a = 1;`，我们就去 Scope 里查：“`a` 是谁？”。

---

### 下一章预告

理论很丰满，现实很骨感。
要写代码解析这些递归结构并不容易。

下一章 **[15. 声明解析器实现](15_decl_parser.md)**，我们将编写 `DeclParser`。
我们将亲手实现 `parse_decl_specifier` 和 `parse_declarator`，彻底征服 C 语言最复杂的语法高地。