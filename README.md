# CCLoneDetector (C-Clone-Detector)

🎓 **写给 C 语言初学者的代码查重器实现教程**

> **项目状态**：当前处于 **词法分析 (Lexer/Tokenizer)** 与 **抽象语法树 (AST)** 构建阶段。
> 
> 这是一个用纯 C 语言从零开始编写的 C 语言代码查重工具。本项目旨在帮助理解：
> 1. C 语言源代码是如何被计算机“读懂”的？
> 2. 如何用 C 语言实现动态数组（Vector）等基础数据结构？
> 3. 所谓的“代码查重”，底层原理到底是什么？

---

## 1. 核心原理：我们需要做什么？

想象一下，你和你的同学写了两个版本的 `Hello World`。

如果你把 `int a = 10;` 改成了 `int b = 10;`，对于人类来说，这是同一段逻辑。但对于计算机来说，字符 `a` 和 `b` 是完全不同的 ASCII 码。

为了检测出这种“换皮”抄袭，我们需要把代码转换成一种**指纹**。

CCLoneDetector 的工作流如下：

1. **Tokenizer (词法分析器)**：读字符 (`i`, `n`, `t`) -> 读单词 (`int`, `identifier`, `number`)。
2. **Normalizer (归一化)**：忽略变量名区别（把 `a` 和 `b` 都视为 `ID`），忽略空格和注释，重构代码先后顺序。
3. **Fingerprinting (指纹提取)**：使用 N-Gram 和哈希算法（MinHash）计算代码的相似度。

**目前本项目主要完成了第一步：教计算机如何“读单词”。**

---

## 2. 核心模块源码导读

对于初学者，阅读源码可能会迷路。请按照以下顺序阅读本项目代码：

### 第一关：动态数组 `Vector`
> **文件**：`vector.h` / `vector.c`

C 语言没有 Python list 或 C++ vector 那样自动扩容的数组。我们需要自己造轮子。
*   **难点**：理解 `malloc` 和 `realloc`。
*   **核心逻辑**：
    *   `data`：指向堆内存的指针。
    *   `size`：当前存了多少个元素。
    *   `capacity`：当前申请的内存能装多少元素。
    *   **扩容策略**：当 `size == capacity` 时，申请一块两倍大的新内存，把旧数据搬过去。

### 第二关：词法分析器 `Tokenizer`
> **文件**：`tokenizer.c`, `tokenizer_impl/*.c`

这是目前最精彩的部分。它的作用是将源代码字符串切割成一个个 **Token (记号)**。

#### 核心概念：光标 (Cursor)
我们在 `Tokenizer` 结构体中维护了一个“光标” `pos`，指向当前处理到的字符。

我们主要通过两个动作来移动光标：
1. **Peek (看一眼)**：`src[pos]`。只看当前字符是什么，不移动光标。用于判断下一步做什么。
2. **Advance (吃掉)**：`src[pos++]`。把当前字符“吃”进来，并将光标移到下一位。

#### 状态机逻辑
在 `next()` 函数中，我们通过 `peek()` 看到的字符决定进入哪个处理函数：

- 如果是 `a-z` 或 `_` -> 进入 `tokenize_keyword` 解析关键字或变量名。
- 如果是 `0-9` -> 进入 `tokenize_number`。
- 如果是 `'` -> 进入 `tokenize_char` 解析字符
- 如果是 `"` -> 进入 `tokenize_string` 解析字符串
- 如果是 `+`, `-`, `*` 等 -> 进入 `tokenize_operator` 解析运算符
- 如果是 `#` -> 进入 `tokenize_preprocessor` 解析预处理命令
- 如果是文件结尾 -> 进入 `tokenize_eof` 解析文件结束符
- 如果是未知内容 -> 进入 `tokenize_unknwon` 解析未知符号

> **代码赏析**：
> 看看 `tokenize_literal.c` 中的 `tokenize_number` 函数。它是如何利用 `while(is_digit(...))` 循环，一口气把 "12345" 读成一个完整的 Token 的？

### 第三关：抽象语法树 `ASTNode`
> **文件**：`ast_node.h` / `ast_node.c`

代码不仅是列表，它是有层次的结构（比如 `if` 里面套了 `Block`，`Block` 里面又有代码）。
我们使用 **结构体 + 联合体 (struct + union)** 来定义这种树状结构。

- **Union 的妙用**：一个节点可能是 `AST_BINARY` (二元运算)，也可能是 `AST_IF` (if语句) 等。它们需要的字段不同，用 `union` 可以节省内存并实现多态。

---

## 3. 目录结构说明

```text
ccd/
├── include/              # 头文件 (对外公开的 API)
│   ├── tokenizer.h       # 词法分析器接口
│   ├── vector.h          # 动态数组接口
│   └── ...
├── src/                  # 源代码实现
│   ├── main.c            # 程序入口：读取文件并调用分析器
│   ├── tokenizer.c       # Tokenizer 主逻辑 (状态调度)
│   ├── vector.c          # Vector 实现
│   ├── utils.c           # 辅助函数 (如 is_digit)
│   ├── tokenizer/        # Tokenizer 的具体分类实现
│   │   ├── token.c       # Token 定义与打印
│   │   ├── tokenize_literal.c  # 处理数字、字符、字符串、关键字和变量名
│   │   ├── tokenize_space.c    # 预处理命令、空格和未知情况
│   │   └── tokenize_operator.c # 处理运算符 (+, -, <<= 等)
│   └── ir/   # AST 解析的具体实现
│       └── ast_node.c        # AST 节点生成与内存释放
└── CMakeLists.txt        # 构建脚本
```

---

## 4. 编译与运行

本项目使用 **CMake** 构建（这是 C/C++ 工业界最常用的构建工具，比手写 gcc 命令更规范）。

### 前置要求
*   C 编译器 (GCC 或 Clang)
*   CMake (版本 >= 3.5)

### 构建步骤

```bash
# 1. 创建构建目录（保持源码目录整洁）
mkdir build
cd build

# 2. 生成 Makefile
cmake ..

# 3. 编译
make
```

### 运行测试
编译成功后，`build` 目录下会生成 `ccd_cli` 可执行文件。
准备一个 C 语言文件（比如 `test.c`），然后运行：

```bash
./ccd_cli test.c
```

你将看到源代码被神奇地转换成了 Token 列表：

```text
   1:1     INT           "int"
   1:5     IDENTIFIER    "main"
   1:9     LEFT_PAREN    "("
   1:10    RIGHT_PAREN   ")"
   1:12    LEFT_BRACE    "{"
   ...
```

---

## 5. 动手练习 (TODO List)

如果你想通过本项目学习 C 语言，可以尝试完成以下任务：

1. **[入门]**：在 `tokenizer_impl/tokenize_operator.c` 中，目前我们支持了 C 语言的大部分运算符。试着找找有哪些被遗漏了？
2. **[中级]**：目前的 Tokenizer 遇到 `//` 注释会怎么处理？阅读 `tokenize_space.c`，尝试实现对 `/* ... */` 多行注释的跳过逻辑。
3. **[高级]**：尝试在 `tokenizer.c` 中添加错误处理。如果遇到源代码里出现了非法字符（比如 `@`），程序会崩溃还是报错？

---

## 6. 学习资源推荐

- **《C 程序设计语言 (K&R)》**：学习 C 语言语法的圣经。
- **Crafting Interpreters (构建解释器)**：一本非常棒的免费在线书，教你从零写编译器（本项目深受其启发）。

---

## License

MIT License. 自由学习，自由修改。