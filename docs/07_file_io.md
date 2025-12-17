# 07. 第一道工序：文件读取 (I/O) 与 命令行参数

**本章目标**：
1.  **I/O 组合拳**：掌握 `fopen`, `fseek`, `ftell`, `rewind`, `fread` 的标准读取套路。
2.  **字符串的秘密**：为什么读取文件时申请内存必须要 `size + 1`？
3.  **命令行交互**：学会使用 `argc` 和 `argv`，让程序能接受用户输入的文件名。
4.  **工程化**：创建 `utils` 模块封装辅助函数。

---

## 1. 为什么读取文件这么麻烦？

在 Python 里，读取文件只需要一行：`text = open("main.c").read()`。
但在 C 语言里，我们需要像外科手术一样精准操作：

1.  **打开** 文件流 (`fopen`)。
2.  **询问** 文件有多大？（因为我们需要先 `malloc` 对应大小的内存）。
3.  **回到** 文件开头 (`rewind`)。
4.  **读取** 内容到内存 (`fread`)。
5.  **封口**（手动添加字符串结束符 `\0`）。
6.  **关闭** 文件 (`fclose`)。

漏掉任何一步，程序都可能崩溃、读出乱码或产生内存泄漏。

---

## 2. 创建工具模块 (Utils)

我们不希望 `main.c` 变得臃肿，所以我们将文件读取这种“脏活累活”放入一个新的模块 `utils` 中。

### 第一步：创建头文件 `include/utils.h`

```c
#pragma once

/**
 * @brief 读取整个文件内容到内存字符串中
 * 
 * @param path 文件路径 (例如 "test.c")
 * @return char* 指向文件内容的堆内存指针 (记得用完后 free!)
 *               如果读取失败，返回 NULL
 */
char *read_file(const char *path);
```

### 第二步：实现功能 `src/utils.c`

请在 `src` 目录下新建 `utils.c`。这里是本章的核心代码：

```c
#include "utils.h"
#include <stdio.h>  // fopen, fclose, fseek, ftell...
#include <stdlib.h> // malloc, free

char *read_file(const char *path) {
    // 1. 打开文件
    // "rb": read binary (二进制读取)
    // 为什么要用 "b"？虽然后面读的是文本，但在获取文件大小时，
    // 二进制模式能保证跨平台准确性（防止 Windows 把 \r\n 自动转义导致字节数对不上）。
    FILE *file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "Error: Could not open file \"%s\"\n", path);
        return NULL;
    }

    // 2. 计算文件大小 (三步走战略)
    fseek(file, 0, SEEK_END);  // A. 把光标移到文件末尾
    long length = ftell(file); // B. 看看现在光标在第几个字节（即文件大小）
    rewind(file);              // C. 把光标移回文件开头 (如果不移回去，后面啥都读不到)

    // 检查是否是空文件或读取错误
    if (length < 0) {
        fclose(file);
        return NULL;
    }

    // 3. 申请内存
    // 关键点：为什么是 length + 1？
    // 因为 C 语言字符串必须以 '\0' (空字符) 结尾。
    // 文件本身通常不包含 '\0'，我们要为它预留一个位置，否则 printf 会打印出乱码甚至崩溃。
    char *buffer = (char *)malloc(length + 1);
    if (!buffer) {
        fprintf(stderr, "Error: Out of memory\n");
        fclose(file);
        return NULL;
    }

    // 4. 读取内容
    // fread(目标内存, 每次读多大, 读几次, 文件流)
    // 这里我们一次读 1 个字节，读 length 次
    size_t read_size = fread(buffer, 1, length, file);
    
    // 5. 封口 (添加结束符)
    // 这一步至关重要！将缓冲区最后一个字节设为 0。
    buffer[read_size] = '\0';

    // 6. 关闭文件
    fclose(file);

    return buffer;
}
```

---

## 3. 认识 main 函数的参数 (argc, argv)

以前我们写 `main` 都是 `int main() { ... }`。
但真正的命令行工具，需要接受用户输入，比如：
`./ccd_cli test.c`

这时候我们需要标准的 `main` 签名：

```c
int main(int argc, char *argv[])
```

*   **`argc` (Argument Count)**: 参数个数。
    *   如果你输入 `./ccd_cli`，argc = 1（程序名本身算一个）。
    *   如果你输入 `./ccd_cli test.c`，argc = 2。
*   **`argv` (Argument Vector)**: 参数字符串数组。
    *   `argv[0]` = "./ccd_cli" (程序路径)
    *   `argv[1]` = "test.c" (第一个参数)

---

## 4. 更新 `main.c` 进行测试

让我们修改 `src/main.c`，把之前的 Vector 测试删掉（或者注释掉），改成一个简单的文件读取器。

```c
#include <stdio.h>
#include <stdlib.h>
#include "utils.h" // 引入刚才写的读取模块

int main(int argc, char *argv[]) {
    // 1. 检查参数
    // 如果用户只输了 ./ccd_cli 而没输文件名，提示用法
    if (argc < 2) {
        printf("Usage: %s <source_file.c>\n", argv[0]);
        return 1;
    }

    // 2. 获取文件名
    const char *filename = argv[1];
    printf("Reading file: %s ...\n", filename);

    // 3. 读取文件
    char *content = read_file(filename);
    if (!content) {
        return 1; // 读取失败
    }

    // 4. 打印文件内容 (验证读取是否成功)
    printf("--- File Content Start ---\n");
    printf("%s\n", content);
    printf("--- File Content End ---\n");

    // 5. 释放内存 (好习惯)
    free(content);

    return 0;
}
```

---

## 5. 更新构建系统

别忘了告诉 CMake 我们新增了 `src/utils.c`。

修改根目录下的 `CMakeLists.txt`：

```cmake
set(SOURCES
    src/main.c
    src/vector.c
    src/utils.c   # <--- 新增这一行
)
```

---

## 6. 运行测试

1.  **编译项目**：
    ```bash
    cd build
    cmake .. && cmake --build .
    ```

2.  **准备测试文件**：
    在 `build` 目录下随便创建一个文本文件，比如 `hello.c`，写点内容进去：
    ```c
    int main() { return 0; }
    ```

3.  **运行程序**：
    *   Windows: `.\ccd_cli.exe hello.c`
    *   Mac/Linux: `./ccd_cli hello.c`

**预期输出**：
```text
Reading file: hello.c ...
--- File Content Start ---
int main() { return 0; }
--- File Content End ---
```

如果你能看到文件内容被正确打印出来，恭喜你！你的程序已经具备了与磁盘文件交互的能力。

---

## 核心知识点总结

1.  **`fopen` 模式**：读文本文件也建议用 `"rb"` 读取，避免 Windows 下 `\r\n` 自动转换导致的 `ftell` 字节数与 `malloc` 不匹配问题。
2.  **`fseek` + `ftell`**：这是 C 语言中获取文件大小的标准“杂技”。
3.  **`malloc(len + 1)`**：**永远不要忘记给字符串留结束符 `\0` 的位置**。这是 C 语言最容易被忽视的细节，也是很多缓冲区溢出漏洞的根源。
4.  **CLI 交互**：通过 `argc/argv`，你的程序不再是只能打印 "Hello World" 的自闭玩具，而是能响应用户指令的工具。

---

### 下一章预告

到目前为止，我们已经搭建好了所有的 **基础设施**：
*   工程结构 (CMake)
*   数据容器 (Vector)
*   数据定义 (Token)
*   IO 通道 (File Read)

接下来的旅程将进入 **核心算法区域**。
计算机读到了刚才那串字符串 `int main() { ... }`，但对它来说，那只是一堆没有意义的字符。

下一章 **[08. 词法分析原理](08_tokenizer_concept.md)**，我们将探讨如何把这堆乱七八糟的字符，切分成计算机能理解的单词（Token）。我们将学习 **有限状态机 (FSM)** 的设计思想。