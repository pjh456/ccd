# 06. 造轮子现场：手写泛型 Vector 实现

**本章目标**：

1.  **核心技能**：熟练使用 `memcpy` 进行内存拷贝。
2.  **指针魔法**：理解 `(char*)ptr + offset` 这种指针运算的含义。
3.  **完整实现**：编写 `src/vector.c`，完成初始化、添加、读取、释放功能。
4.  **单元测试**：验证我们的 Vector 既能存 `int` 也能存 `Token`。

---

## 1. 搭建代码骨架

请在 `src/` 目录下新建文件 `vector.c`。

首先，我们需要引入必要的头文件：
```c
#include "vector.h" // 引入我们上一章定义的结构体和函数声明

#include <stdlib.h> // 为了使用 malloc, free, realloc
#include <string.h> // 为了使用 memcpy
#include <stdio.h>  // 为了报错打印 (可选)
```

---

## 2. 实现构造函数 (New)

我们要创建一个新的 Vector。

这里要注意，**Vector 结构体本身** 和 **它存储的数据** 是分开两块内存的。
```c
Vector *vector_new(size_t ele_size) {
    // 1. 先申请 Vector 结构体本身的内存 (那个背包)
    Vector *vec = (Vector *)malloc(sizeof(Vector));
    
    // 2. 初始化成员
    vec->data = NULL;   // 一开始没有数据，所以钥匙是空的
    vec->size = 0;      // 库存为 0
    vec->capacity = 0;  // 总容量为 0
    vec->ele_size = ele_size; // 记录每个元素有多大 (例如 4 字节)
    
    return vec;
}
```

---

## 3. 实现析构函数 (Free)

这一步至关重要。

如果我们只 `free(vec)`，那么 `vec->data` 指向的那一大堆数据就**泄露**了。

必须由内而外释放。

```c
void vector_free(Vector *vec) {
    if (vec == NULL) return;

    // 1. 先释放背包里的东西 (如果申请过的话)
    if (vec->data != NULL) {
        free(vec->data);
    }

    // 2. 再把背包本身扔掉
    free(vec);
}
```

---

## 4. 核心难点：Push Back (扩容与拷贝)

这是最复杂的一个函数。

我们需要处理两件事：

1. **扩容**：如果满了，就调用 `realloc`。
2. **拷贝**：把用户传进来的数据，按字节复制到 `data` 里。

### 难点解析：指针运算

因为 `data` 是 `void*`，编译器不知道它指向什么类型，所以不能进行加减运算（`data + 1` 是非法的或未定义的）。

我们需要把它强转为 `char*`（字节指针）。

**在 C 语言中，`char` 永远是 1 字节。**

所以：`目标地址 = 起始地址 + (第几个 * 每个多大)`
```c
int vector_push_back(Vector *vec, void *elem) {
    // Step 1: 检查是否需要扩容
    if (vec->size == vec->capacity) {
        // 扩容策略：如果是 0，就变成 4；否则翻倍
        size_t new_cap = (vec->capacity == 0) ? 4 : vec->capacity * 2;
        
        // 申请新内存 (总字节数 = 容量 * 单个元素大小)
        void *new_data = realloc(vec->data, new_cap * vec->ele_size);
        
        if (!new_data) {
            return 0; // 内存不足，返回失败
        }
        
        vec->data = new_data;
        vec->capacity = new_cap;
    }

    // Step 2: 计算写入位置 (Pointer Arithmetic)
    // 目标地址 = 基地址 + (当前个数 * 单个大小)
    // 必须转为 char* 才能按字节计算偏移！
    char *dest_addr = (char *)vec->data + (vec->size * vec->ele_size);

    // Step 3: 内存拷贝 (memcpy)
    // 从 elem 把数据拷贝到 dest_addr，拷 ele_size 这么多字节
    memcpy(dest_addr, elem, vec->ele_size);

    // Step 4: 更新计数
    vec->size++;
    return 1; // 成功
}
```

---

## 5. 实现读取 (Get)

同样利用指针运算，返回对应位置的地址。

```c
void *vector_get(Vector *vec, size_t idx) {
    // 1. 安全检查
    if (idx >= vec->size) {
        return NULL; // 越界了
    }

    // 2. 计算地址并返回
    return (char *)vec->data + (idx * vec->ele_size);
}
```

---

## 6. 验证时刻：是骡子是马牵出来溜溜

我们要验证 Vector 的**泛型能力**。

修改 `src/main.c`，把之前的测试代码替换为 Vector 测试：

```c
#include <stdio.h>
#include "../include/vector.h"
#include "../include/token.h" // 借用一下之前的 Token 定义

int main() {
    // ============================
    // 测试 1: 存简单的 int
    // ============================
    printf("--- Test 1: Integer Vector ---\n");
    Vector *v_int = vector_new(sizeof(int)); // 告诉它存 int

    for (int i = 0; i < 10; i++) {
        vector_push_back(v_int, &i); // 注意：必须传地址！
    }

    for (size_t i = 0; i < v_int->size; i++) {
        // vector_get 返回 void*，必须强转为 int* 才能解引用
        int *p = (int *)vector_get(v_int, i);
        printf("%d ", *p);
    }
    printf("\n");
    vector_free(v_int); // 记得释放！

    // ============================
    // 测试 2: 存复杂的 Struct Token
    // ============================
    printf("\n--- Test 2: Token Vector ---\n");
    Vector *v_token = vector_new(sizeof(Token)); // 告诉它存 Token

    Token t1 = {T_KEYWORD, "int", 3, 1, 1};
    Token t2 = {T_IDENTIFIER, "main", 4, 1, 5};

    vector_push_back(v_token, &t1); // 发生了一次 memcpy
    vector_push_back(v_token, &t2); // 又发生了一次 memcpy

    // 修改原始的 t1，看看 Vector 里会不会变？
    // 预期：不会变，因为是深拷贝（copy-by-value）
    t1.line = 999; 

    Token *pt1 = (Token *)vector_get(v_token, 0);
    printf("Token 1: %.*s (Line %d)\n", (int)pt1->length, pt1->start, pt1->line);
    
    Token *pt2 = (Token *)vector_get(v_token, 1);
    printf("Token 2: %.*s (Line %d)\n", (int)pt2->length, pt2->start, pt2->line);

    vector_free(v_token);

    return 0;
}
```

### 编译运行

**注意**：这次我们新增了 `src/vector.c`，必须更新 `CMakeLists.txt`！

打开 `CMakeLists.txt`，修改 `SOURCES` 变量：
```cmake
set(SOURCES
    src/main.c
    src/vector.c  # <--- 新增这一行
)
```

然后重新构建：
```bash
cd build
cmake ..
cmake --build .
./ccd_cli
```

**预期输出**
```text
--- Test 1: Integer Vector ---
0 1 2 3 4 5 6 7 8 9 

--- Test 2: Token Vector ---
Token 1: int (Line 1)  <-- 这里还是 Line 1，说明拷贝成功
Token 2: main (Line 1)
```

---

## 核心知识点总结

1.  **`void*` 的两面性**：它给了我们泛型的自由，但也剥夺了直接运算的能力。必须转为 `char*` 才能进行字节级操作。
2.  **`memcpy` 的本质**：它不管你是不是结构体，它只管搬运字节。这就是为什么 `vector_push_back` 可以接受任何类型。
3.  **内存所有权**：我们的 Vector 是 **拥有** 数据的（因为发生了拷贝）。当 `vector_free` 时，数据随之销毁。
4.  **接口设计**：我们在 `.h` 中暴露接口，在 `.c` 中隐藏实现。外部使用者不需要知道 `capacity` 是怎么翻倍的，这就是 **封装**。

---

### 下一章预告

神兵利器（Vector）已经打造完毕。

现在我们有了存数据的容器，有了数据结构的定义（Token）。

是时候教计算机如何“阅读”文件了。

下一章 **[07. 文件读取](07_file_io.md)**，我们将编写一个工具函数，把源代码文件从硬盘一次性读入内存，为词法分析做准备。