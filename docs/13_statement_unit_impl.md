# 13. 递归扫描实战：从平面到立体

**本章目标**：

1.  **扫描器上下文**：实现 `UnitScanner`，它像 Tokenizer 一样有一个光标，但它是跳跃在 Token 数组上的。
2.  **递归下降算法**：理解 `scan_compound` 调用 `scan_unit`，`scan_unit` 又调用 `scan_if` 的套娃逻辑。
3.  **核心调度器**：编写 `scan_unit`，它是 StatementUnit 层的交通指挥官。
4.  **集成测试**：在 `main.c` 中打印出代码的树形结构。

---

## 1. 扫描器上下文 (UnitScanner)

和词法分析类似，我们需要一个对象来记录“当前解析到第几个 Token 了”。

在 `src/unit_scanner.c` 中，我们首先实现基础设施（构造、销毁、Peek/Next）。

**注意**：我们需要先引入 `unit_scanner_impl/unit_scanner_impl.h`，这是我们为了内部函数共享而定义的头文件（见文末附录）。

```c
#include "unit_scanner.h"
#include "unit_scanner_impl/unit_scanner_impl.h"
#include "vector.h"
#include <stdlib.h>

UnitScanner *unit_scanner_new(Vector *tokens)
{
    UnitScanner *us = malloc(sizeof(*us));
    us->tokens = tokens; // 持有 Token 列表的引用
    us->pos = 0;         // 光标归零
    return us;
}

void unit_scanner_free(UnitScanner *us)
{
    if (!us) return;
    free(us);
}

// 偷看当前 Token
Token *peek_token(UnitScanner *us)
{
    if (us->pos >= us->tokens->size) {
        // 返回一个临时的 EOF Token，防止越界崩溃
        // (在实际工程中通常会有一个全局静态的 EOF Token)
        static Token t_eof = {T_EOF, NULL, 0, 0}; 
        return &t_eof;
    }
    return (Token *)vector_get(us->tokens, us->pos);
}

// 消耗并返回当前 Token
Token *next_token(UnitScanner *us)
{
    Token *t = peek_token(us);
    if (t->type != T_EOF) {
        us->pos++;
    }
    return t;
}
```

---

## 2. 调度总管：`scan_unit`

这是本章的灵魂函数。它根据当前 Token 的类型，决定调用哪个具体的扫描函数。

请继续在 `src/unit_scanner.c` 中添加：

```c
// 前向声明，因为各个 scan_xxx 可能会互相调用
StatementUnit *scan_unit(UnitScanner *us);

// ... 这里需要包含所有 scan_xxx 的声明，建议放在 unit_scanner_impl.h 中 ...

StatementUnit *scan_unit(UnitScanner *us)
{
    Token *t = peek_token(us);

    // 1. 也是最重要的一步：防止死循环
    if (t->type == T_EOF) return NULL;

    // 2. 根据关键字分发任务
    switch (t->type) {
        // --- 复合语句 ---
        case T_LEFT_BRACE: // '{'
            return scan_compound(us);
        case T_SEMICOLON:  // ';'
            return scan_empty(us);

        // --- 控制流 ---
        case T_IF:
            return scan_if(us);
        case T_SWITCH:
            return scan_switch(us);
        case T_WHILE:
            return scan_while(us);
        case T_DO:
            return scan_do_while(us);
        case T_FOR:
            return scan_for(us);
        
        // --- 跳转 ---
        case T_RETURN:
            return scan_return(us);
        case T_BREAK:
            return scan_break(us);
        case T_CONTINUE:
            return scan_continue(us);
        case T_GOTO:
            return scan_goto(us);

        // --- 特殊 ---
        case T_PREPROCESS:
            return scan_preprocessor(us);
        case T_CASE:
            return scan_case(us);
        case T_DEFAULT:
            return scan_default(us);
            
        // --- 默认情况 ---
        // 如果不是上述关键字，那它大概率是：
        // 1. 变量声明: int a = 1;
        // 2. 表达式: i++;
        // 3. 标签: label:
        default:
            // 这里需要一点前瞻逻辑来区分 Label 和 普通表达式
            // 如果是 "Identifier :" 则是 Label
            if (t->type == T_IDENTIFIER) {
                // 偷看下一个
                size_t next_idx = us->pos + 1;
                if (next_idx < us->tokens->size) {
                    Token *next = vector_get(us->tokens, next_idx);
                    if (next->type == T_COLON) {
                        return scan_label(us);
                    }
                }
            }
            // 否则，统统当作声明或表达式处理
            return scan_decl_or_expression(us);
    }
}
```

---

## 3. 入口函数：`scan_file`

我们需要一个函数来启动整个过程，它通常解析的是整个文件，也就是最外层的 `Unit`。在 C 语言中，最外层通常可以看作是一个隐式的 Compound Unit（包含全局变量、函数定义等）。

但在我们的 `StatementUnit` 设计中，函数定义 `int main() { ... }` 也会被当作 `scan_decl_or_expression` 的一部分（因为 `{` 之前的部分）或者是某种复合结构。

为了简化，我们定义 `scan_file` 为：**持续调用 `scan_unit` 直到文件结束，并将它们收集到一个大的 Compound Unit 中。**

```c
StatementUnit *scan_file(UnitScanner *us)
{
    // 创建一个列表来存顶层的单元
    Vector *units = vector_new(sizeof(StatementUnit*));
    size_t start_pos = us->pos;

    while (peek_token(us)->type != T_EOF) {
        StatementUnit *unit = scan_unit(us);
        if (unit) {
            vector_push_back(units, &unit);
        } else {
            // 防御性编程：如果 scan_unit 返回 NULL 但没到 EOF，
            // 说明遇到了无法解析的东西，强制跳过一个 token 以免死循环
            next_token(us);
        }
    }

    // 将整个文件包装成一个 Compound Unit
    return make_compound_statement_unit(
        vector_slice(us->tokens, start_pos, us->pos),
        units
    );
}
```

---

## 4. 详解：递归下降是如何工作的？

让我们以解析 `if (a > 0) { return; }` 为例，看看调用栈是如何变化的：

1.  **`scan_unit`** 看到 `if`，调用 **`scan_if`**。
2.  **`scan_if`**：
    *   吃掉 `if`。
    *   吃掉 `(`。
    *   调用 **`scan_decl_or_expression`** 解析 `a > 0`。
    *   吃掉 `)`。
    *   调用 **`scan_unit`** 解析 Body。
        *   **`scan_unit`** 看到 `{`，调用 **`scan_compound`**。
            *   **`scan_compound`** 吃掉 `{`。
            *   **`scan_compound`** 进入循环，调用 **`scan_unit`**。
                *   **`scan_unit`** 看到 `return`，调用 **`scan_return`**。
                    *   **`scan_return`** 生成 Return Unit。
                *   **`scan_unit`** 返回。
            *   **`scan_compound`** 看到 `}`，结束。
            *   **`scan_compound`** 返回 Compound Unit。
        *   **`scan_unit`** 返回。
    *   **`scan_if`** 组装 If Unit，返回。

这就是 **递归** 的美妙之处。我们不需要手写复杂的栈管理，C 语言的函数调用栈帮我们完成了嵌套结构的解析。

---

## 5. 补充头文件 `unit_scanner_impl.h`

为了让分散在各个 `.c` 文件里的 `scan_xxx` 能互相调用，你需要创建这个头文件：

**`include/unit_scanner_impl/unit_scanner_impl.h`**:

```c
#pragma once
#include "unit_scanner.h"
#include "statement_unit.h"

// 所有的扫描子函数声明
StatementUnit *scan_unit(UnitScanner *us); // 总入口

StatementUnit *scan_compound(UnitScanner *us);
StatementUnit *scan_empty(UnitScanner *us);
StatementUnit *scan_preprocessor(UnitScanner *us);

StatementUnit *scan_if(UnitScanner *us);
StatementUnit *scan_switch(UnitScanner *us);
StatementUnit *scan_case(UnitScanner *us);
StatementUnit *scan_default(UnitScanner *us);

StatementUnit *scan_while(UnitScanner *us);
StatementUnit *scan_do_while(UnitScanner *us);
StatementUnit *scan_for(UnitScanner *us);

StatementUnit *scan_break(UnitScanner *us);
StatementUnit *scan_continue(UnitScanner *us);
StatementUnit *scan_return(UnitScanner *us);

StatementUnit *scan_label(UnitScanner *us);
StatementUnit *scan_goto(UnitScanner *us);

StatementUnit *scan_decl_or_expression(UnitScanner *us);
```

---

## 6. 集成测试：在 `main.c` 中打印语法树

现在，让我们见证奇迹。

修改 `src/main.c`：

```c
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "tokenizer.h"
#include "vector.h"
#include "unit_scanner.h"
#include "unit_scanner_impl/statement_unit.h" // 为了使用 print_statement_unit

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    // 1. 读取 & 词法分析
    char *src = read_file(argv[1]);
    Vector *tokens = tokenize_all(src);
    
    // 2. 单元扫描 (Unit Scanning)
    UnitScanner *us = unit_scanner_new(tokens);
    StatementUnit *syntax_tree = scan_file(us);

    // 3. 打印树结构
    printf("=== Syntax Tree ===\n");
    // print_statement_unit 是我们在 statement_unit.c 里实现的打印函数
    // 第二个参数 1 表示打印出具体的 token 内容，0 表示只打印结构
    print_statement_unit(syntax_tree, 1); 

    // 4. 清理
    statement_unit_free(syntax_tree); // 递归释放整棵树
    unit_scanner_free(us);
    // vector_free(tokens); // 注意：syntax_tree 持有了 tokens，释放 tree 时可能已经处理了
    // 这里的内存管理略复杂：tokens 被 syntax_tree 的叶子节点引用。
    // 如果 statement_unit_free 会释放 tokens，这里就不用释放了。
    // 根据第 12 章的代码，statement_unit_free 确实会 free tokens vector。
    // 但原始的 tokens vector (由 tokenize_all 创建的) 需要处理。
    // 最佳实践：UnitScanner 只借用 tokens，不拥有。
    // StatementUnit 采用 slice (浅拷贝 vector，但引用同样的 token 指针)。
    // 最终：我们需要手动释放 tokens 容器本身，和 src。
    vector_free(tokens); // 释放最外层的 vector 容器
    free(src);
    
    return 0;
}
```

---

## 7. 更新 CMakeLists.txt

把所有新文件都加进去！

```cmake
set(SOURCES
    src/main.c
    src/utils.c
    src/vector.c
    src/token.c
    src/tokenizer.c
    src/tokenize_space.c
    src/tokenize_literal.c
    src/tokenize_operator.c
    
    # 新增的 Unit Scanner 相关
    src/unit_scanner.c
    src/statement_unit.c
    src/unit_scanner_compound.c
    src/unit_scanner_conditional.c
    src/unit_scanner_loop.c
    src/unit_scanner_decl_or_expr.c
    src/unit_scanner_break.c
    src/unit_scanner_label.c
)
```

---

### 下一章预告

运行程序，你应该能看到一个缩进良好的、类似于目录树的结构，清晰地展示了你的 C 代码逻辑骨架。

但是，现在的 `DECL_OR_EXPR` 还是一个黑盒子。
`int *a[10];` 对我们来说只是一串 Token。

为了进行更深度的查重（例如区分变量类型），我们需要打开这个黑盒子，去理解 C 语言中最晦涩难懂的部分——**声明 (Declaration)**。

下一章 **[14. C 语言声明的奥秘](14_decl_concept.md)**，我们将挑战 `int *(*foo)(int)` 这种令人望而生畏的语法。