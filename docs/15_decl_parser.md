# 15. 声明解析器实现：挑战 C 语言语法的巅峰

**本章目标**：

1.  **分而治之**：将声明解析拆分为“类型说明符解析”和“声明符解析”。
2.  **递归下降**：实现 `parse_declarator`，处理指针 `*` 的右结合性。
3.  **后缀处理**：实现 `parse_direct_declarator`，处理数组 `[]` 和函数 `()` 的左结合性。

> **注意**：由于项目进度暂时停留于此，关于 `struct`/`union`/`enum`/`typedef`的解析暂时还未补充。

---

## 1. 解析器的架构

我们在 `src/decl_parser.c` 中定义了解析器的入口。

这里的输入不再是 Token 流，而是 **StatementUnit 流**。
因为我们在第 13 章已经把代码切成了 `SUT_DECL_OR_EXPR` 这样的粗粒度单元。

```c
// DeclParser 上下文
struct DeclParser {
    Vector *stmts;   // 语句单元列表
    size_t stmt_pos; // 当前解析到第几个语句
    size_t token_pos;// 当前语句解析到第几个 Token
};
```

入口函数 `parse_decl_statement` 的逻辑非常直观：
1.  **解析 Specifier**：先看看类型是什么（`int`, `static`...）。
2.  **解析 Declarator List**：再看看变量名是什么（`a`, `*b`, `c[10]`...）。

```c
DeclUnit *parse_decl_statement(DeclParser *dp) {
    // 1. 解析类型说明符 (int, const, struct A...)
    DeclSpecifier *spec = parse_decl_specifier(dp);
    
    // 2. 解析声明列表 (a, *b=1, c[10])
    Vector *decls = parse_decl_initializer_list(dp);
    
    return make_declaration_decl_unit(spec, decls);
}
```

---

## 2. 难点一：解析 Declarator (指针的递归)

声明符的解析核心在于：**指针 `*` 是右结合的，且优先级低于 `[]` 和 `()`**。

比如 `int *a[10]`，实际上是 `(int*) (a[10])` 吗？**错！**
它是 `int * (a[10])`。即：`a` 是一个数组，它的元素是 `int *`。

所以在代码中 (`src/decl_parser_declarator.c`)，我们将解析分为两层：

1.  **`parse_declarator`**: 处理前缀的指针 `*`。
2.  **`parse_direct_declarator`**: 处理标识符 `a` 以及后缀 `[]`, `()`。

### 指针解析逻辑

```c
Declarator *parse_declarator(DeclParser *dp) {
    // 1. 看看有没有星号 '*'
    // 如果有多个星号 (int **p)，循环读取
    Vector *ptr_buffer = vector_new(sizeof(unsigned));
    while (peek(T_STAR)) {
        consume(T_STAR);
        // 读取 const/volatile 限定符 (int * const p)
        unsigned qual = parse_pointer_qualifiers(dp); 
        vector_push_back(ptr_buffer, &qual);
    }

    // 2. 解析核心部分 (变量名, 数组, 函数)
    Declarator *decl = parse_direct_declarator(dp);

    // 3. 把刚才攒的星星，一层层包上去
    // 注意：最先读到的星号在最外层，所以要倒序包裹？
    // C语言语法：int * * p; 
    // p 是 pointer -> pointer -> int
    // 我们读到了两个星号。
    for (int i = ptr_buffer->size - 1; i >= 0; i--) {
        unsigned qual = ...;
        decl = make_pointer_declarator(decl, qual);
    }
    
    return decl;
}
```

---

## 3. 难点二：Direct Declarator (后缀的迭代)

处理完前缀 `*` 后，我们进入 `parse_direct_declarator`。
这里处理的是：
1.  **变量名**：`a`
2.  **括号分组**：`(a)`
3.  **后缀**：`[10]`, `(int x)`

后缀是可以无限连缀的，比如 `a[10][20]()`。
这需要一个 `while` 循环不断吞噬后缀。

```c
Declarator *parse_direct_declarator(DeclParser *dp) {
    Declarator *decl = NULL;
    
    // 1. 先处理“头”：要么是标识符，要么是括号
    if (peek(T_IDENTIFIER)) {
        decl = make_identifier_declarator(...);
    } else if (peek(T_LEFT_PAREN)) {
        // 遇到括号，递归调用 parse_declarator
        // 这就是 int (*p)[10] 能够正确解析的关键！
        decl = make_group_declarator(parse_declarator(dp));
        expect(T_RIGHT_PAREN);
    }

    // 2. 循环处理“尾巴”：数组 [] 和 函数 ()
    while (1) {
        if (peek(T_LEFT_BRACKET)) {
            // 解析数组：a[10]
            decl = parse_array_suffix(dp, decl); 
            // 注意：parse_array_suffix 内部会将 decl 包裹进一个新的 ArrayDeclarator
        } 
        else if (peek(T_LEFT_PAREN)) {
            // 解析函数：f(int x)
            decl = parse_function_suffix(dp, decl);
        } 
        else {
            break; // 没有后缀了，收工
        }
    }
    
    return decl;
}
```

### 关键：为什么要传入 `decl`？

注意 `parse_array_suffix(dp, decl)` 这个调用。
我们把当前的 `decl` 传进去，函数内部会创建一个新的 `ArrayDeclarator`，并把旧的 `decl` 设为 `inner`。

比如 `int a[10]`：
1. 先解析 `a` -> 得到 `Ident(a)`。
2. 遇到 `[`，调用 `parse_array`，传入 `Ident(a)`。
3. `parse_array` 创建 `Array`，设置 `Array->inner = Ident(a)`。
4. 返回 `Array`。

---

## 4. 实战：解析 `int *(*f)(int)`

让我们跟踪一下这个复杂的解析过程：

1.  **`parse_decl_statement`**: 读到 `int`，生成 `Specifier(int)`。
2.  **`parse_declarator`**:
    *   没读到 `*`，进入 **`parse_direct_declarator`**。
    *   没读到 ID，读到 `(`，进入 Group 处理。
        *   递归调用 **`parse_declarator`**。
            *   读到 `*`，缓存起来。
            *   调用 **`parse_direct_declarator`**。
                *   读到 `f`，生成 `Ident(f)`。
            *   返回 `Pointer(Ident(f))`。
        *   Group 结束，得到 `Group(Pointer(Ident(f)))`。
    *   回到外层 Direct，现在有了“头”。
    *   看到后缀 `(`，解析参数列表 `(int)`。
        *   生成 `Function`，把刚才的 Group 包进去。
    *   得到 `Function(Group(Pointer(Ident(f))))`。
3.  **`parse_declarator`**:
    *   看最外层，前面有 `*` 吗？也就是 `int *...` 的那个星号。
    *   有，生成 `Pointer`，把刚才那一坨包进去。
4.  **最终结果**:
    `Pointer` -> `Function` -> `Group` -> `Pointer` -> `Ident(f)`。
    
    翻译成人话：**一个指向（函数的）指针，该函数返回（指向int的）指针**。

---

## 5. 小结

这一章的代码量虽然只有几百行，但包含了编译原理中处理 **优先级 (Precedence)** 和 **结合性 (Associativity)** 的经典技巧：

*   **前缀运算**（指针）通过递归函数入口处理。
*   **后缀运算**（数组/函数）通过循环处理。
*   **括号优先级** 通过递归调用自身处理。

如果你能完全理解 `src/decl_parser_declarator.c` 中的代码，那么恭喜你，你已经跨过了 C 语言解析器最困难的一座大山。

---

### 下一章预告

我们已经把代码解析成了非常精细的结构：`DeclUnit`。
但是，`if (a>0)` 的条件表达式 `a>0` 目前还躺在 `SUT_DECL_OR_EXPR` 这个黑盒子里。

为了计算代码指纹，我们需要把这些表达式也拆解成树形结构。

下一章 **[16. 抽象语法树设计 (AST)](16_ast_design.md)**，我们将设计最终的 AST 节点结构，将所有的 StatementUnit 和 DeclUnit 统一起来，构建一棵完整的、巨大的语法树。这将是我们进行特征提取的最终形态。