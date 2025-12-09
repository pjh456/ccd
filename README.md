# CCLoneDetector

**CCLoneDetector** 是一个使用 **C 语言实现的 C 代码查重工具**，

基于 `tokenizer → normalizer → k-gram → minhash` 的流水线，

对 C 源代码进行轻量、快速、可扩展的相似度分析。

它适合作为：
- 教学环境中的查重工具  
- CI 中的重复代码检测插件  
- 代码聚类、代码相似度研究工具  
- 自定义 IR / Token 分析阶段的底层组件

## Features

- **轻量实现**：纯 C，无外部依赖，可嵌入任意工程  
- **四阶段流水线**：  
  - `Tokenizer`：词法切分  
  - `Normalizer`：标识符/常量归一化  
  - `K-Gram Builder`：n-gram 序列生成  
  - `MinHash Signature`：快速近似相似度计算  
- **可扩展 API**：所有阶段均模块化  
- **跨平台**：支持 Linux / macOS / Windows  
- **可嵌入 CLI 或库使用**

## Build

项目使用 CMake 构建：

```bash
mkdir build
cd build
cmake ..
make
```

编译后可获得：

- `ccd_cli` —— 命令行工具

## Usage (CLI)
```bash
./ccd_cli file1.c file2.c
```

输出会包含两个文件的 MinHash signature 以及近似相似度。

## Directory Structure
```
ccd/
├── include/          # 头文件（公共 API）
├── src/              # 内部实现
├── tests/            # 单元测试
├── CMakeLists.txt
└── README.md
```

## Pipeline Overview

### 1. Tokenizer
将 C 源代码切分为 token 序列，包含：
- 标识符
- 关键字
- 字符串 / 字面值
- 符号 / 运算符

### 2. Normalizer
对 token 序列进行归一化处理，例如：
- 标识符统一替换
- 字面值归一化
- 删除无意义 token

### 3. K-Gram
基于 token 生成长度为 `k` 的滑动窗口序列。

### 4. MinHash
为 k-gram 序列生成固定长度的 MinHash signature，用于近似判断相似度。

## Status

当前处于 早期开发阶段。
欢迎 issue / PR / 建议。

## License
MIT License.