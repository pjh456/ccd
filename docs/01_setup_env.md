# 01. 告别 Dev-C++：现代化 C 开发环境搭建

**本章目标**：

1. 理解为什么我们要抛弃老旧的 IDE（集成开发环境）。
2. 安装并配置业界标准的开发三件套：**VSCode + GCC + CMake**。
3. 学会使用**命令行 (Terminal)** 编译和运行程序，而不是点击绿色的播放按钮。

---

## 为什么要折腾环境？

很多同学在大一上课时，老师可能会发一个蓝色的图标叫 **Dev-C++**，或者让你安装 **Visual Studio 2019/2022**（紫色图标的巨无霸）。

- **Dev-C++**：太老了，它的编译器甚至不支持很多 C99/C11 的新特性，且无法管理多文件项目。
- **Visual Studio**：太强了，强到帮你把所有脏活累活都干了。点击“运行”，它默默帮你链接了库、配置了路径。**但作为学习者，如果你不亲手做这些，你永远不知道程序是怎么跑起来的。**

在本教程中，我们将使用 **VSCode**（编辑器）+ **GCC**（编译器）+ **CMake**（构建系统）的组合。这是目前开源界和工业界最通用的 C/C++ 开发方案。

---

## 第一步：安装编译器 (GCC/Clang)

C 语言代码是文本，计算机看不懂，需要**编译器**把它翻译成二进制机器码。

### Windows 用户

Windows 是最麻烦的，因为它没有自带 C 编译器。我们推荐安装 **MinGW-w64**。

1.  **下载**：访问 [winlibs.com](https://winlibs.com/)。
2.  **选择版本**：下载最新版本的 **UCRT runtime** (Zip archive)。
    > 如果不确定，选 `GCC 13.x.x + LLVM ... (Win64) - Zip`。
3.  **解压**：将压缩包解压到一个简单路径，不要包含中文或空格！
    - 推荐：`C:\mingw64`
    - 禁止：`D:\软件\新建文件夹（2）\mingw`
4.  **配置环境变量 (Path)**（**这是最关键的一步！**）：
    - 按 `Win` 键，搜索“编辑系统环境变量”。
    - 点击“环境变量” -> 在“系统变量”里找到 **Path** -> 点击“编辑”。
    - 点击“新建”，输入你的 `bin` 目录路径，例如：`C:\mingw64\bin`。
    - **一路点击“确定”保存。**

### macOS 用户

macOS 自带了 Clang 编译器（由于兼容性极高，我们可以把它当 GCC 用）。

1.  打开终端 (Terminal)。
2.  输入命令：`xcode-select --install`
3.  在弹出的窗口中点击安装即可。

### Linux 用户

如果你在用 Linux，那你应该已经会了。
```bash
sudo apt update
sudo apt install build-essential cmake
```

### 验证安装

打开你的终端（Windows 下按 `Win+R` 输入 `cmd`，或使用 PowerShell）：

输入：
```bash
gcc --version
```
如果出现类似 `gcc (MinGW-w64 ...) 13.2.0` 的字样，恭喜你，第一步成功了！
> 如果提示“不是内部或外部命令”，请回头检查 Windows 的 **环境变量** 配置。

---

## 第二步：安装构建工具 (CMake)

GCC 只能编译单个或少量的文件。当我们的项目有几十个文件时，手动敲 `gcc main.c vector.c tokenizer.c ...` 会累死人。

**CMake** 是一个“构建系统的生成器”，它能帮我们自动管理这些编译流程。

1. **下载**：访问 [CMake 官网下载页](https://cmake.org/download/)。
2. **安装**：
    - Windows: 下载 `.msi` 安装包。**注意：安装时务必选择 "Add CMake to the system PATH for all users"（添加到环境变量）**。
    - macOS: `brew install cmake` (如果你有 Homebrew) 或下载 `.dmg`。
3. **验证**：
    打开新终端，输入：
    ```bash
    cmake --version
    ```

---

## 第三步：安装编辑器 (VSCode)

1.  下载并安装 [Visual Studio Code](https://code.visualstudio.com/)。
2.  打开 VSCode，点击左侧的方块图标（扩展商店）。
3.  搜索并安装以下插件：
    - **C/C++** (Microsoft 出品，提供代码高亮和提示)
    - **CMake** (twxs 出品，提供语法高亮)
    - **CMake Tools** (Microsoft 出品，方便后续构建)

---

## 第四步：Hello World 与 命令行基础

我们要写的第一个程序，**不要** 直接点 VSCode 右上角的“三角形”运行按钮。我们要学会像工程师一样在命令行操作。

### 1. 准备工作区

在你的电脑上创建一个文件夹，命名为 `ccd_project`。

在 VSCode 中点击 `File -> Open Folder`，打开这个文件夹。

### 2. 编写代码

新建文件 `main.c`：

```c
#include <stdio.h>

int main() {
    printf("Hello, CCD!\n");
    return 0;
}
```

### 3. 打开终端

在 VSCode 顶部菜单栏点击 `Terminal -> New Terminal`。你会看到下方弹出了一个小黑框。

### 4. 常用命令速查

在编译之前，先学几个最基本的命令（Windows/Mac/Linux 通用）：

- `ls` (Windows PowerShell) 或 `dir` (Windows CMD)：**列出** 当前目录下的文件。
- `cd <目录名>`：**进入** 指定目录。
- `cd ..`：**返回** 上一级目录。
- `mkdir <目录名>`：**创建** 新目录。

### 5. 手动编译运行

在终端中输入以下命令：
```bash
# 调用 gcc 编译器
# -o main 指明输出的文件名叫 main (Windows下会自动变成 main.exe)
# main.c 是你的源代码
gcc main.c -o main

# 运行程序
# Windows (PowerShell):
.\main.exe
# Mac/Linux:
./main
```

如果你看到屏幕打印出了 `Hello, CCD!`，恭喜你！你已经脱离了 IDE 的拐杖，学会了裸机编译。

---

## 第五步：初识 CMake (选修预热)

在未来的章节中，我们将完全依赖 CMake。这里先演示一下它是如何工作的。

在 `main.c` 同级目录下，新建一个文件 `CMakeLists.txt`（文件名严格区分大小写，一个字都不能错）：
```cmake
# 设定 CMake 最低版本要求
cmake_minimum_required(VERSION 3.10)

# 设定项目名称
project(CCD_Tutorial)

# 设定 C 标准为 C11
set(CMAKE_C_STANDARD 11)

# 告诉 CMake：我要生成一个可执行文件
# 名字叫 ccd_cli
# 源代码来自 main.c
add_executable(ccd_cli main.c)
```

然后，在终端中执行标准的 **CMake 构建流程**（请背诵这套连招）：
```bash
# 1. 创建构建目录（防止编译垃圾污染源码）
mkdir build

# 2. 进入构建目录
cd build

# 3. 生成构建系统 (.. 代表上一级目录，即 CMakeLists.txt 所在的位置)
cmake ..

# 4. 真正开始编译
cmake --build .
```

执行完后，你会发现 `build` 目录下多了一个 `ccd_cli` (或 `ccd_cli.exe`)。运行它，效果和刚才一样。

---

## 常见问题 (FAQ)

**Q: 终端提示 `gcc: command not found` 或 `无法将 gcc 识别为 cmdlet...`**
A: 99% 是因为你没配置好 Windows 的 **Path 环境变量**，或者配置完后没有 **重启 VSCode**。请重新检查第一步，并重启电脑。

**Q: 为什么要用 `build` 文件夹？**
A: 编译过程中会产生 `.o`、`.obj`、`Makefile` 等一大堆临时文件。如果都堆在源码目录里，会非常乱。把它们都关进 `build` 文件夹里，想清理时直接把 `build` 删了就行，非常清爽。

---

### 下一章预告

环境搭好了，但我们的代码不能全写在 `main.c` 里。下一章 **[02. 工程结构](02_project_layout.md)** 将教你如何像整理衣柜一样整理你的代码，以及如何把 `.h` 和 `.c` 分开写。