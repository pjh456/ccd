# 11. 复杂 Token 处理：查表与贪婪匹配

**本章目标**：

1.  **关键字识别**：理解为什么 `int` 是关键字，而 `integral` 是变量名（边界检查）。
2.  **字符串解析**：处理转义字符 `\`，确保字符串能正确闭合。
3.  **贪婪匹配 (Maximal Munch)**：实现运算符解析的核心算法，确保 `>>=` 不会被切碎。

---

## 1. 关键字 vs 标识符：查表法 (`tokenize_literal.c`)

在 `src/tokenize_literal.c` 中，我们处理所有的“字面量”和“单词”。

### 核心难题：前缀冲突

假设代码里写了 `interface`。
Tokenizer 读到 `int` 时，如果不小心，就会把它当成关键字 `T_INT` 切分出去，留下 `erface` 变成一个奇怪的变量名。

**解决方案**：我们在查表匹配时，必须检查 **单词边界**。

### 代码实现

我们定义了一个关键字表 `kw_table`，然后实现 `tokenize_keyword`：

```c
Token *tokenize_keyword(Tokenizer *tk)
{
    // 1. 遍历关键字表
    for (const KwEntry *e = kw_table; e->kw; e++)
    {
        size_t len = strlen(e->kw);
        const char *p = tk->src + tk->pos;

        // 2. 匹配字符串内容 (例如 "int")
        if (strncmp(p, e->kw, len) == 0)
        {
            // 3. 【关键】边界检查！
            // 检查匹配到的关键字后面，是不是还连着字母或数字？
            // 如果源码是 "integer"，匹配 "int" 后，下一个字符是 'e' (is_alnum)
            // 这说明它不是独立的关键字，不能切分。
            if (!is_alnum(p[len]))
            {
                // 确认为关键字
                Token *t = make_token(tk, e->type, p, len);
                tk->pos += len;       // 移动光标
                tk->stus.col += len;  // 更新列号
                return t;
            }
        }
    }

    // 4. 如果没匹配上任何关键字，那它就是普通的标识符 (Identifier)
    // 规则：一直吃到非字母数字为止
    const char *start = tk->src + tk->pos;
    while (is_alnum(peek(tk)))
        advance(tk);
        
    // ... 生成 T_IDENTIFIER ...
}
```

---

## 2. 字符串与转义：状态机中的状态机

在 C 语言中，字符串以 `"` 开头，以 `"` 结尾。
最麻烦的是 **转义字符**：字符串 `"Hello \"World\""` 中间的 `"` 不应该导致字符串结束。

我们通过 `tokenize_string` 来处理（`tokenize_char` 逻辑类似）：

```c
Token *tokenize_string(Tokenizer *tk)
{
    // ... 记录开始位置 ...
    advance(tk); // 1. 吃掉开头的双引号 "

    while (peek(tk) != '\"') // 只要没遇到结束引号
    {
        char ch = peek(tk);
        
        // 安全检查：防止文件突然结束，或者字符串跨行（标准C不支持字符串直接跨行）
        if (ch == '\0' || ch == '\n') break;

        // 2. 处理转义字符
        if (ch == '\\') {
            advance(tk); // 吃掉反斜杠
            // 再吃掉下一个字符（不管它是什么，都视为普通字符）
            // 例如 \" 变成了 "，\\ 变成了 \
            if (peek(tk) != '\0') advance(tk);
            continue;
        }

        advance(tk); // 普通字符，吃掉
    }
    
    advance(tk); // 3. 吃掉闭合的双引号 "
    // ... 生成 Token ...
}
```

> **注意**：我们这里的 Tokenizer 保存的是源码的 **原始片段 (Raw Snippet)**。
> 如果源码是 `"a\nb"`, 我们的 Token 内容就是 `"a\nb"` (4个字符)，而不是转义后的 `a`、换行、`b` (3个字符)。这对于代码查重和静态分析来说通常足够了。

---

## 3. 运算符：最大吞噬法则 (Maximal Munch)

这是词法分析中最著名的算法规则。

**场景**：源码是 `vector<vector<int>>` 和 `cin >> var`。
- 前者的 `>>` 是两个 `>` (模板右括号)。
- 后者的 `>>` 是一个 `>>` (右移/流输入运算符)。

**规则**：Tokenizer **总是** 尝试匹配 **最长** 的合法符号。

在 `src/tokenize_operator.c` 中，我们并没有用复杂的 Trie 树，而是用了一个简单暴力的 **线性扫描 + 打擂台** 算法：

```c
Token *tokenize_operator(Tokenizer *tk)
{
    const char *p = tk->src + tk->pos;
    const OpEntry *best = NULL;
    size_t best_len = 0;

    // 遍历所有已知的运算符 (op_table)
    for (const OpEntry *e = op_table; e->op; e++)
    {
        size_t len = strlen(e->op);

        // 尝试匹配
        if (strncmp(p, e->op, len) == 0)
        {
            // 打擂台：如果你比当前找到的 best 还长，你就赢了
            // 例如：当前 best 是 ">" (len 1)
            // 现在匹配到了 ">>=" (len 3) -> 更新 best 为 ">>="
            if (len > best_len)
            {
                best = e;
                best_len = len;
            }
        }
    }

    if (best) {
        // ... 生成 best->type 的 Token ...
    } else {
        // 遇到无法识别的符号（如 @, $ 等 C 语言不用的符号）
        // 生成 T_UNKNOWN 并跳过
    }
    return t;
}
```

这种写法的好处是：**不需要** 对 `op_table` 进行严格的长度排序。
无论表里的顺序是 `>` 先还是 `>>=` 先，算法总能找到最长的那个。

---

## 4. 阶段性验收：跑起来！

现在，我们的 Tokenizer 已经具备了完整的战力。
让我们修改 `src/main.c`，读取一个真实的 C 文件，并打印出所有的 Token。

### 编写测试文件 `test.c`
在 `build` 目录下新建 `test.c`：
```c
int main() {
    // 这是一行注释
    char *s = "Hello \"World\"";
    int a = 10;
    a += 20; // 测试运算符 +=
    if (a >= 30) {
        return 0;
    }
}
```

### 修改 `src/main.c`

```c
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "tokenizer.h"
#include "vector.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    // 1. 读取文件
    char *src = read_file(argv[1]);
    if (!src) return 1;

    // 2. 词法分析
    Vector *tokens = tokenize_all(src);
    if (!tokens) {
        free(src);
        return 1;
    }

    // 3. 打印结果
    printf("Successfully tokenized %zu tokens:\n", tokens->size);
    for (size_t i = 0; i < tokens->size; i++) {
        Token *t = (Token *)vector_get(tokens, i);
        print_token(t);
        
        // 记得释放 Token 内部的字符串 (如果不是 String View 模式)
        token_free(t); 
    }

    // 4. 清理
    vector_free(tokens);
    free(src);
    return 0;
}
```

### 编译运行

```bash
cd build
cmake .. && cmake --build .
./ccd_cli test.c
```

**预期输出**：
```text
INT (int) at 1:1
IDENTIFIER (main) at 1:5
LEFT_PAREN (() at 1:9
RIGHT_PAREN ()) at 1:10
LEFT_BRACE ({) at 1:12
CHAR (char) at 3:5
STAR (*) at 3:10
IDENTIFIER (s) at 3:11
ASSIGN (=) at 3:13
STRING ("Hello \"World\"") at 3:15
SEMICOLON (;) at 3:30
INT (int) at 4:5
IDENTIFIER (a) at 4:9
...
PLUS_ASSIGN (+=) at 5:7
...
GREATER_EQUAL (>=) at 6:11
...
EOF () at ...
```

如果你看到了精准的 `PLUS_ASSIGN` (+=) 和 `GREATER_EQUAL` (>=)，而不是被拆散的 `+` `=`，说明你的贪婪匹配工作正常！

---

## 核心知识点总结

1.  **关键字边界**：匹配关键字时，必须向后看一眼，防止匹配到变量名的前缀。
2.  **转义字符处理**：在解析字符串时，遇到 `\` 要跳过下一个字符的边界检查。
3.  **贪婪匹配 (Maximal Munch)**：词法分析器总是渴望匹配最长的 Token，这是解决运算符歧义的标准法则。

---

### 下一章预告

恭喜！你已经完成了一个功能完备的 C 语言词法分析器。
现在我们手里有了一串漂亮的 Token 流。

但是，对于查重来说，`for (int i=0; i<10; i++)` 和 `while(i<10)` 长得完全不一样，但逻辑却是一样的。
如果我们直接比对 Token 流，查重效果会很差。

我们需要把 Token 组装成更粗粒度的结构。

下一章 **[12. 语句单元设计 (StatementUnit)](12_statement_unit_concept.md)**，我们将引入一个新的概念，在不构建完整语法树的前提下，提取代码的 **“逻辑骨架”**。这将是我们抗代码混淆的第一道防线。