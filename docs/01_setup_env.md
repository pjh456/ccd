# 01. 告别 Dev-C++：现代化 C 开发环境搭建

**本章目标**：
1. 理解 **编辑器 (Editor)**、**编译器 (Compiler)** 和 **构建系统 (Build System)** 的分工。
2. 搭建业界标准的 **VSCode + GCC + CMake** 开发环境。
3. 掌握**命令行 (Terminal)** 基础，学会像工程师一样编译程序。

---

## 为什么要折腾环境？

很多同学在大一上课时，用的可能是 **Dev-C++** 或者 **Code::Blocks**。
它们就像“微波炉”：你把食物（代码）放进去，按一个按钮，叮的一声，饭就熟了。

但是，如果你想成为一名厨师，你不能只用微波炉。你需要掌握 **刀具（编辑器）** 和 **炉灶（编译器）**。

- **VSCode**：当前最流行的代码编辑器，颜值高、插件多。（它是你的刀）
- **GCC / Clang**：负责把文本代码翻译成机器码。（它是你的炉灶）
- **CMake**：负责指挥炉灶，管理复杂的烹饪流程（先切菜还是先烧油？）。（它是厨师长）

在本教程中，我们将彻底抛弃“点击绿色播放键”的习惯，学会掌控代码构建的每一个细节。

---

## 第一步：安装编译器 (GCC)

计算机看不懂 C 语言，它只认识 `0` 和 `1`。我们需要编译器充当翻译。

### Windows 用户 (最易踩坑，请仔细阅读)

Windows 没有自带 C 编译器，我们需要安装 **MinGW-w64**。

1.  **下载**：访问 [winlibs.com](https://winlibs.com/)。
    - 找到最新的 **UCRT runtime** 版本。
    - 选择 `Zip archive` (无需安装，解压即用)。
    - *示例：GCC 13.2.0 + LLVM ... (Win64) - Zip*
2.  **解压 (关键！)**：
    - 将压缩包解压到一个 **路径简短、无中文、无空格** 的目录。
    - **推荐**：`C:\mingw64`
    - **禁止**：`D:\软件 install\新建文件夹(2)\编译器` (这会导致无数奇怪的报错)
3.  **配置环境变量 Path (生死攸关的一步)**：
    - 这一步是为了让终端在任何地方都能找到 `gcc` 命令。
    - 按 `Win` 键，搜索并打开 `编辑系统环境变量`。
    - 点击右下角的 `环境变量` 按钮。
    - 在下方的 **"系统变量"** 区域，找到名为 `Path` 的条目，选中并点击 `编辑`。
    - 点击 `新建`，粘贴你的 `bin` 目录路径（例如 `C:\mingw64\bin`）。
    - **连续点击三次“确定”保存退出。**

### macOS 用户

macOS 自带 Clang 编译器（兼容 GCC 命令）。
打开终端 (Terminal)，输入以下命令安装开发工具包：
```bash
xcode-select --install
```

### Linux 用户

作为 Linux 用户，你应该已经轻车熟路了：
```bash
# Ubuntu/Debian
sudo apt update && sudo apt install build-essential cmake

# Fedora
sudo dnf install gcc gcc-c++ make cmake
```

### 验证安装

打开终端（Windows 下按 `Win+R` 输入 `cmd`，或使用 PowerShell）：
输入：
```bash
gcc --version
```
如果屏幕上出现了版本号（如 `gcc (MinGW-w64...) 13.2.0`），恭喜你，最难的一步完成了！
> **错误排除**：如果提示 `'gcc' 不是内部或外部命令`，说明你的环境变量配置错了。请重新检查路径是否正确，并重启终端。

---

## 第二步：安装构建工具 (CMake)

当项目只有 1 个文件时，手动敲 `gcc` 很简单。但当你有 50 个文件时，你需要自动化工具。

1. **下载**：访问 [CMake 官网](https://cmake.org/download/)。
2. **安装**：
    - **Windows**: 下载 `.msi` 安装包。
    - **注意**：安装过程中，**务必** 选择 **"Add CMake to the system PATH for all users"**。
    - **macOS**: 推荐使用 Homebrew: `brew install cmake`。
3. **验证**：
    打开新终端，输入 `cmake --version`。

---

## 第三步：安装编辑器 (VSCode)

1.  下载并安装 [Visual Studio Code](https://code.visualstudio.com/)。
2.  打开 VSCode，点击左侧边栏的方块图标（扩展商店）。
3.  搜索并安装以下“C 开发三件套”插件：
    - **C/C++** (Microsoft) - 提供代码补全、调试功能。
    - **CMake** (twxs) - 提供 CMake 语法高亮。
    - **CMake Tools** (Microsoft) - 让 VSCode 识别 CMake 项目。

---

## 第四步：Hello World 与 命令行基础

现在，我们不使用任何“一键运行”插件，纯手工体验一次编译过程。

### 1. 准备工作区
在你的电脑上创建一个文件夹 `ccd_project`。
**注意：项目路径中不要包含中文！** (例如不要放在 `C:\用户\张三\代码`)。
在 VSCode 中点击 `File -> Open Folder` 打开它。

### 2. 编写代码
新建 `main.c`：
```c
#include <stdio.h>

int main() {
    printf("Hello, CCD!\n");
    return 0;
}
```

### 3. 打开内置终端
在 VSCode 顶部菜单点击 `Terminal -> New Terminal`。你会看到下方出现了一个小黑框。

### 4. 终端生存指南
这是程序员的通用语言，请熟记这几个命令：

| 作用 | Windows (PowerShell) | Mac / Linux |
| :--- | :--- | :--- |
| **列出文件** | `ls` 或 `dir` | `ls` |
| **进入目录** | `cd 目录名` | `cd 目录名` |
| **返回上级** | `cd ..` | `cd ..` |
| **创建目录** | `mkdir 目录名` | `mkdir 目录名` |

### 5. 手工编译
在终端输入：
```bash
# 语法：gcc [源文件] -o [输出文件名]
gcc main.c -o main
```
如果是 Windows，你会发现多了一个 `main.exe`；Linux/Mac 则是 `main`。

### 6. 运行程序
```bash
# Windows
.\main.exe

# Mac / Linux
./main
```
看到 `Hello, CCD!` 了吗？恭喜，你已经具备了最基础的工程能力。

---

## 第五步：初识 CMake (工程化的开始)

在接下来的课程中，我们将完全依赖 CMake 来管理项目。

在 `main.c` 同级目录下，新建文件 `CMakeLists.txt`（文件名区分大小写，一个字都不能错）：

```cmake
# 1. 规定 CMake 最低版本
cmake_minimum_required(VERSION 3.10)

# 2. 项目名称
project(CCD_Tutorial)

# 3. 设置 C 标准 (C11)
set(CMAKE_C_STANDARD 11)

# 4. 生成可执行文件
# 语法：add_executable(程序名 源文件1 源文件2 ...)
add_executable(ccd_cli main.c)
```

### 标准构建流程 (Standard Build Workflow)

请背诵以下 **4 行连招**，以后我们每写一点代码，都要做这套动作：

```bash
# 1. 创建构建目录 (把编译产生的垃圾文件都关进去，保持源码整洁)
mkdir build

# 2. 进入构建目录
cd build

# 3. 生成构建系统 (告诉 CMake 去上一级目录找 CMakeLists.txt)
cmake ..

# 4. 编译！
cmake --build .
```

执行完毕后，在 `build` 目录下运行你的程序：
```bash
# Windows
.\ccd_cli.exe
# Mac/Linux
./ccd_cli
```

---

## 常见问题 FAQ

**Q: 终端提示 `gcc: command not found`？**
A: 你的环境变量 Path 没配好，或者配置完没重启 VSCode。请重试第一步。

**Q: 为什么要进 `build` 目录编译？**
A: 这叫 "Out-of-source build"（源码外构建）。如果你直接在源码目录编译，会生成一大堆 `.o`, `.make`, `.cache` 文件，把你的代码库弄得乱七八糟。用 `build` 目录，想清理时直接把 `build` 文件夹删了就行，非常清爽。

**Q: 包含 `<stdio.h>` 时有波浪线报错？**
A: 按 `Ctrl+Shift+P`，输入 `C/C++: Edit Configurations (UI)`，在 `Compiler path` 一栏选择你刚才安装的 `gcc.exe` 的路径。

---

### 下一章预告

环境搭好了，但我们的代码不能全塞在 `main.c` 里。
下一章 **[02. 工程结构设计](02_project_layout.md)**，我们将学习如何像整理衣柜一样整理代码，以及 `include` 和 `src` 的正确用法。