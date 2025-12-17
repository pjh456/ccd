# 12. 语句单元设计 (StatementUnit)：抗混淆的第一道防线

**本章目标**：

1.  **架构思维**：理解 **粗粒度解析 (Coarse-Grained Parsing)** —— 为什么我们不直接一步到位生成 AST？
2.  **数据结构**：学习 C 语言中 `struct` + `union` + `enum` 的经典组合，用于表示多态树节点。
3.  **核心抽象**：定义 `StatementUnit`，将代码视为“骨架”与“血肉”的结合。

---

## 1. 为什么需要“语句单元”？

在上一章，我们把代码切成了 Token 流：
`int` `main` `(` `)` `{` `return` `0` `;` `}`

如果要直接把这一串 Token 变成精准的抽象语法树 (AST)，难度非常大。C 语言的语法极其复杂，比如：
*   `T * a;` 到底是 **乘法**（T 乘以 a）还是 **指针声明**（T 类型的指针 a）？这需要符号表。
*   `x = (y) * z;` 是强制类型转换还是乘法？

但在 **代码查重** 这个场景下，我们有一个独特的优势：**我们其实不太关心具体的表达式算得对不对。**

我们关心的是 **代码的骨架结构**：
*   这两个函数是不是都有 3 层 `for` 循环？
*   它们的 `if-else` 嵌套结构是不是一模一样？

因此，我们引入一个中间层概念：**StatementUnit (语句单元)**。

**策略：**
1.  对于 `if`, `while`, `{}` 这种 **结构化语句**，我们要精准解析，提取骨架。
2.  对于 `int a = b + c;` 这种 **计算语句**，我们先“囫囵吞枣”，把它打包成一个黑盒子，暂不深究内部逻辑。

---

## 2. 定义 StatementUnit

根据提供的 `statement_unit.h`，我们采用了 C 语言实现多态（Polymorphism）的标准范式：**Tagged Union**。

### 第一步：给所有语句发身份证 (Enum)

```c
typedef enum {
    SUT_COMPOUND,     // { ... } 代码块
    SUT_EMPTY,        // ; 空语句
    SUT_PREPROCESSOR, // #include ... 

    SUT_DECL_OR_EXPR, // 核心黑盒：声明或表达式 (int a=1; 或 a++;)

    // 控制流
    SUT_IF, SUT_SWITCH, SUT_CASE, SUT_DEFAULT,
    SUT_WHILE, SUT_DO_WHILE, SUT_FOR,

    // 跳转
    SUT_CONTINUE, SUT_BREAK, SUT_RETURN,
    SUT_LABEL, SUT_GOTO
} StatementUnitType;
```

### 第二步：设计“百变金刚”结构体 (Struct + Union)

这是 C 语言最高频的高级数据结构写法。

一个 `StatementUnit` 既可能是 `If`，也可能是 `While`。为了节省内存，我们使用 `union` 让它们共享同一块空间。

```c
struct StatementUnit
{
    Vector *tokens;         // 该语句包含的原始 Token 列表 (保留证据)
    StatementUnitType type; // 它是哪种类型？

    union // 联合体：同一时刻只能是以下其中一种
    {
        // 如果 type == SUT_COMPOUND
        struct {
            Vector *units; // 里面包含了一堆子语句 (递归定义)
        } compound_stmt;

        // 如果 type == SUT_IF
        struct {
            StatementUnit *cond;      // 条件 (黑盒)
            StatementUnit *then_body; // if 分支 (递归)
            StatementUnit *else_body; // else 分支 (递归，可为 NULL)
        } if_stmt;

        // 如果 type == SUT_WHILE
        struct {
            StatementUnit *cond;
            StatementUnit *body;
        } while_stmt;

        // ... 其他类型的专属数据 ...
        
        // 如果 type == SUT_DECL_OR_EXPR
        // 它不需要额外字段，因为它只有 tokens，没有子节点
    };
};
```

**设计图解：**

```text
StatementUnit (Type: SUT_IF)
┌──────────────────────────────────┐
│ tokens: [if, (, a, >, 0, ), ...] │
│ type: SUT_IF                     │
│ union:                           │
│   if_stmt:                       │
│     ├── cond: [a, >, 0] (SUT_DECL_OR_EXPR)
│     ├── then_body: { ... } (SUT_COMPOUND)
│     └── else_body: NULL          │
└──────────────────────────────────┘
```

---

## 3. 核心分类解析

我们的目标是把平铺的 Token 流，折叠成一棵树。

### 1. 容器类：Compound Statement
这是树的 **枝干**。
遇到 `{` 时，我们创建一个 `SUT_COMPOUND`，然后递归地去扫描里面的内容，直到遇到 `}`。它里面存的是一个 `Vector<StatementUnit*>`。

### 2. 骨架类：If / Loop / Switch
这是树的 **节点**。
遇到 `if` 时，我们知道后面紧跟的是 `(条件)`，然后是 `Body`，可能有 `else`。
*   我们将 `(条件)` 视为一个子单元（通常是表达式）。
*   我们将 `Body` 视为另一个子单元（通常是 Compound）。

### 3. 叶子类：Decl or Expr
这是树的 **树叶**。
比如 `int a = 10;` 或 `func();`。
在这一层级，我们**不关心**它具体是什么。只要它以分号 `;` 结尾，或者不包含 `{}` 结构，我们就把它看作一个整体。

**这就是“抗混淆”的奥义：** 即使作弊者把 `int a = 10;` 改成了 `int a = 5 + 5;`，在 StatementUnit 这一层，它们的结构是一模一样的（都是一个 `DECL_OR_EXPR` 节点）。

---

## 4. 内存管理策略

由于 `StatementUnit` 是树形结构，释放内存时必须使用 **递归释放**。

看看 `statement_unit.c` 中的 `statement_unit_free`：

```c
void statement_unit_free(StatementUnit *unit)
{
    if (!unit) return;

    // 1. 先释放自己持有的 Token 引用 (Vector)
    statement_unit_free_tokens(unit->tokens);

    // 2. 根据类型，递归释放子节点
    switch (unit->type)
    {
    case SUT_IF:
        statement_unit_free(unit->if_stmt.cond);
        statement_unit_free(unit->if_stmt.then_body);
        statement_unit_free(unit->if_stmt.else_body);
        break;
    
    case SUT_COMPOUND:
        // 遍历 Vector，释放每一个子语句
        if (unit->compound_stmt.units) {
             for (size_t i = 0; i < unit->compound_stmt.units->size; ++i) {
                 StatementUnit *child = vector_get(unit->compound_stmt.units, i);
                 statement_unit_free(child);
             }
             vector_free(unit->compound_stmt.units);
        }
        break;
        
    // ... 其他 case ...
    }

    // 3. 最后释放自己
    free(unit);
}
```

---

## 5. 准备工作：CMake 与文件创建

我们将源码分为两部分：
1.  `statement_unit.c`: 定义数据结构、构造函数 (`make_xxx`) 和 析构函数。
2.  `unit_scanner.c`: 真正干活的扫描器逻辑（下一章讲）。

请根据提供的源码，创建以下文件结构：

```text
src/
├── statement_unit.c             (核心结构体实现)
├── statement_unit_break.c       (Break/Continue 构造)
├── statement_unit_compound.c    (Compound 构造)
├── statement_unit_conditional.c (If/Switch 构造)
├── statement_unit_decl_or_expr.c
├── statement_unit_label.c
├── statement_unit_loop.c
└── ...
```

记得更新 `CMakeLists.txt`，把这些新文件都加进去。如果你觉得文件太多，也可以先把它们合并成一个 `src/statement_unit.c`，但为了工程整洁，推荐按功能拆分。

---

### 下一章预告

数据结构已经定义好了，树的形状我们也设计好了。

下一章 **[13. 递归扫描实战](13_statement_unit_impl.md)**，我们将编写 `UnitScanner`。
你会看到递归下降（Recursive Descent）算法的雏形：
`scan_compound` 调用 `scan_unit`，`scan_unit` 遇到 `if` 调用 `scan_if`，`scan_if` 内部又调用 `scan_unit` 解析 Body...

这将是整个项目中最具逻辑美感的部分。