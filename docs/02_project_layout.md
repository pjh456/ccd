# 02. 不要把鸡蛋放在同一个篮子里：C 项目工程结构

**本章目标**：

1. 明白为什么不能把几千行代码都写在 `main.c` 里。
2. 掌握标准的 C 语言项目目录结构（`src`, `include`, `build`）。
3. 学会编写适应多文件结构的 `CMakeLists.txt`。

---

## 为什么不管好目录会很惨？

这也是新手最常见的问题。刚开始写代码时，你可能习惯这样做：

```text
my_project/
└── main.c  <-- 里面塞了 2000 行代码，包含了结构体定义、算法实现、主函数...
```

当你写到 500 行的时候，你就会发现：
1. **找代码太难了**：想改个结构体，得鼠标滚轮滚半天。
2. **多人合作是噩梦**：如果你和室友一起写大作业，两个人都改 `main.c`，合并代码时会发生极其惨烈的冲突。
3. **复用性为零**：如果你写了一个很好用的 `Vector`（动态数组），下次想在别的项目里用，你得把那段代码从 `main.c` 里抠出来，极其麻烦。

所以，我们需要 **分家**。

---

## 标准的 C 项目户型图

在工业界，一个标准的 C/C++ 项目通常长这样。请现在就动手，在你的 `ccd_project` 文件夹里创建这些子文件夹：

```text
ccd_project/
├── CMakeLists.txt      # 项目的总管（构建脚本）
├── README.md           # 项目说明书
├── .gitignore          # 告诉 Git 忽略哪些垃圾文件
├── src/                # [Source] 源文件目录 (.c)
│   └── main.c          # 程序入口
├── include/            # [Include] 头文件目录 (.h)
│   └── (暂时为空)
├── build/              # [Build] 编译输出目录 (由 CMake 自动生成)
└── tests/              # [Tests] 测试代码目录 (放单元测试)
```

### 这里的逻辑是：

- **厨房 (`src/`)**：厨师干活的地方。具体的逻辑实现、函数的代码都写在这里的 `.c` 文件里。
- **菜单 (`include/`)**：顾客看的地方。这里存放 `.h` 文件，告诉别人你的代码里有哪些函数可以用，长什么样（接口声明）。
- **垃圾桶 (`build/`)**：编译过程会产生很多中间文件（`.o`, `.exe`），把它们都关在这里，保持项目干净。

---

## 动手搬家

现在，让我们把上一章写的 `main.c` 移动到正确的位置。

1. **移动文件**：将根目录下的 `main.c` 拖动到 `src/` 文件夹里。
2. **清理现场**：如果根目录下有之前编译生成的 `main.exe` 或 `main`，请直接删除。

现在的结构应该是：
```text
ccd_project/
├── CMakeLists.txt
└── src/
    └── main.c
```

---

## 升级 CMakeLists.txt

文件移动后，之前的 `CMakeLists.txt` 就失效了，因为 CMake 找不到 `main.c` 了（它以为在根目录，其实在 `src` 里）。

而且，我们还需要告诉 CMake：“请去 `include` 文件夹里找头文件”。

请修改根目录下的 `CMakeLists.txt`，将其更新为以下内容：

```cmake
# 1. 设定最低版本
cmake_minimum_required(VERSION 3.5)

# 2. 项目名称
project(CCD_Project)

# 3. 设定标准 (C99 是必须的，为了支持 // 注释和变量随处定义)
set(CMAKE_C_STANDARD 11)

# =========================================================
# 核心配置区域
# =========================================================

# 4. [关键] 告诉编译器：头文件去哪里找？
# ${CMAKE_SOURCE_DIR} 代表项目的根目录
# 这样我们在代码里写 #include "xxx.h" 时，编译器会自动去 include/ 文件夹找
include_directories(${CMAKE_SOURCE_DIR}/include)

# 5. [关键] 收集所有的源文件
# 这里我们手动列出 src 目录下的 main.c
# 以后如果加了 vector.c，就在后面接着写 src/vector.c
set(SOURCES
    src/main.c
)

# 6. 生成可执行文件
# ccd_cli 是输出程序的名字
# ${SOURCES} 是上面定义的变量，代表所有源文件
add_executable(ccd_cli ${SOURCES})
```

#### **小技巧**：为什么要用变量 `SOURCES`？
当你的文件变多时，比如有 `src/vector.c`, `src/token.c`, `src/ast.c`...
你只需要在第 5 步那里把它们加进去，第 6 步的代码永远不用改。

---

## 再次编译运行

现在的编译流程和上一章一模一样，但底层逻辑已经变了。

1.  打开终端。
2.  **如果 `build` 文件夹还在，建议先清空它**（避免旧缓存干扰）。
    - Windows: `rd /s /q build` 然后 `mkdir build`
    - Mac/Linux: `rm -rf build && mkdir build`
3.  执行构建连招：

```bash
cd build
cmake ..
cmake --build .
```

4.  运行程序：
    - Windows: `.\Debug\ccd_cli.exe` (注意：有时候 CMake 会多生成一层 Debug 目录) 或 `.\ccd_cli.exe`
    - Mac/Linux: `./ccd_cli`

如果依然输出了 `Hello, CCD!`，恭喜你！你已经成功搭建了一个 **可扩展的工程骨架**。

---

## 附加题：配置 .gitignore (选做)

如果你打算把代码上传到 GitHub / Gitee，这一步非常重要。
一定要防止把 `build` 文件夹上传上去（因为那是你电脑生成的临时文件，别人的电脑不需要，也不能用）。

在根目录新建一个文件名为 `.gitignore` (注意前面有个点)，填入：

```text
# 忽略编译生成目录
build/
.vscode/

# 忽略系统生成文件
.DS_Store
*.exe
*.o
```

这样，当你使用 Git 时，它就会自动无视这些垃圾文件了。

---

## 常见问题 (FAQ)

**Q: 我把 `main.c` 移进去后，CMake 报错 `Cannot find source file`。**
A: 记得保存 `CMakeLists.txt`！并且一定要重新运行 `cmake ..`。CMake 不会自动检测文件移动，必须重新生成构建文件。

**Q: 为什么不用 `file(GLOB ...)` 自动查找所有 `.c` 文件？**
A: 虽然 `file(GLOB SOURCES "src/*.c")` 很方便，但在工程实践中不推荐。因为如果你新增了一个文件，CMake 不会感知到，你必须手动摸一下 CMakeLists.txt 才能触发重编译。**手动列出文件虽然麻烦一点，但更明确、更安全。**
> ~~以上是 AI 说的，实际上我用的就是 GLOB~~

---

### 下一章预告

骨架搭好了，但我们还欠缺很多 C 语言的基础知识。

下一章 **[03. 结构体与类型定义](03_c_struct_typedef.md)**，我们将补习 C 语言中最重要的数据类型概念，并为我们的查重器设计第一个数据结构——**Token**。