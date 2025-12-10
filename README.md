# C Clone Detector (CCD)

![alt text](https://img.shields.io/badge/build-passing-brightgreen)![alt text](https://img.shields.io/badge/language-C11-blue)![alt text](https://img.shields.io/badge/license-MIT-orange)

写给 **C 语言初学者** 的代码查重系统实现教程。

A C-based Code Clone Detector built from scratch for educational purposes.



**CCD** 是一个 **完全使用 C 语言** 从零编写的代码查重工具。

它不依赖任何复杂的第三方库，旨在通过构建一个实际的工程，帮助初学者彻底掌握 C 语言的核心特性与工程化开发方法。

点击进入：[完整开发教程](docs/intro.md)

---

## 为什么要做这个项目？

很多大一同学学完 C 语言后，往往只停留在“刷题”阶段：

- 会写链表，但不知道怎么在多文件项目中管理内存。
- 听说过编译原理，但觉得它遥不可及。
- 习惯了点击 IDE 的“运行”按钮，却不懂 `CMake` 和 `Makefile` 是怎么工作的。

本项目将带你 **造轮子**，实现以下目标：

1. **脱离 IDE 舒适区**：从零编写 `CMakeLists.txt`，掌握现代化构建流程。
2. **数据结构实战**：因为 C 语言没有 `std::vector`，我们将 **手写动态数组**，彻底理解 `malloc/realloc` 和指针算术。
3. **编译器入门**：亲手实现 Tokenizer（词法分析）和 Parser（语法分析），通过 AST（抽象语法树）理解代码的本质。
4. **算法应用**：抛弃暴力的字符串匹配，学习业界通用的 **MinHash + LSH** 算法进行海量代码查重。

---

## 架构概览

我们将构建一个经典的四阶段流水线：

```txt
源代码 Source Code --> Token 流 Tokenizer
Token 流 Tokenizer --> 抽象语法树 AST Parser
抽象语法树 AST Parser --> 归一化指纹 Normalizer
归一化指纹 Normalizer --> 相似度报告 MinHash
```

1. **Tokenizer**: 把 `int main() {` 变成 `[T_INT, T_IDENTIFIER, T_LEFT_PAREN, T_RIGHT_PAREN, T_LEFT_BRACE]`。
2. **Parser**: 理解代码结构，知道这是一个“函数定义”。
3. **Normalizer**: 忽略变量名区别（int a 和 int b 是一样的），提取骨架。
4. **Algorithm**: 使用数学方法计算两段代码的相似程度。

--- 

## 快速开始

### 环境要求

- C 编译器 (GCC / Clang / MSVC)
- CMake (版本 >= 3.5)
- 任意文本编辑器 (VSCode / Vim / CLion)

### 编译与运行

```bash
# 1. 创建构建目录（保持源码整洁）
mkdir build && cd build

# 2. 生成构建系统
cmake ..

# 3. 编译项目
make

# 4. 运行 CLI 工具
./ccd_cli ../tests/sample.c
```

---

## 目录结构

```txt
ccd/
├── docs/                 # 循序渐进的文字教程（核心部分！）
├── include/              # 头文件（对外接口，告诉别人“我有什么”）
├── src/                  # 源文件（内部实现，告诉机器“怎么做”）
├── tests/                # 测试用例（保证代码没写挂）
└── CMakeLists.txt        # 项目的“说明书”
```

## 参与贡献

如果你在学习过程中发现了 Bug，或者觉得教程哪里没讲清楚，欢迎提交 Issue 或 PR！

### License

MIT License.