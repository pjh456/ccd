# 05. 拒绝重复造轮子：通用动态数组 (Vector) 的设计哲学

**本章目标**：

1.  **架构思维**：学习如何将 `realloc` 和内存管理封装在黑盒内部。
2.  **泛型编程**：在没有 C++ Template（模板）的情况下，如何用 `void*` + `ele_size` 实现万能容器？
3.  **核心难点**：**彻底理解“分层所有权”模型** —— 为什么 `vector_free` 之后，你的 `Token` 可能会发生内存泄漏？
4.  **接口设计**：解读 `vector.h`，确立 API 契约。

---

## 1. 为什么我们需要封装？

在上一章，我们写了手动扩容的代码。如果在 `Tokenizer`、`Parser`、`Analyzer` 里每次都手写一遍 `realloc`，不仅代码冗余，而且一旦忘记检查 `NULL` 或者算错大小，程序就会随机崩溃。

我们需要一个 **通用的容器 (Vector)**：
-   **对外**：你只管 `push`，不用管内存够不够。
-   **对内**：它自动处理 `malloc`、`realloc`、`memcpy`。

---

## 2. 泛型设计的核心：`void*` 与 `ele_size`

C 语言没有模板。如果我们想让 Vector 既能存 `int`，又能存 `Token`，还能存 `Token*`，必须利用 **内存的本质**。

在计算机眼中，没有任何类型，只有 **字节 (Bytes)**。

### 结构体设计

我们在 `include/vector.h` 中定义了这样的结构：

```c
typedef struct {
    void *data;       // 1. 仓库的钥匙（指向堆内存起始位置）
    size_t size;      // 2. 实际库存（存了多少个元素）
    size_t capacity;  // 3. 仓库总容量（malloc 申请了多少空间）
    size_t ele_size;  // 4. 【关键】单件商品的大小
} Vector;
```

-   **`void *data`**：这是一个“无类型”指针。它指向一块连续的内存。
-   **`ele_size`**：这是泛型的关键。
    -   如果你存 `int`，`ele_size` 是 4。
    -   如果你存 `Token` 结构体，`ele_size` 可能是 24。
    -   Vector 不需要知道里面存的是什么，它只需要知道 **“每搬运一次，要拷贝多少个字节”**。

当我们要把数据放入 Vector 时，本质上是在做 **内存拷贝 (memcpy)**：

```c
// 伪代码：把数据放入第 i 个格子
// data + (i * 宽度) = 目标地址
void *dest = (char*)vec->data + (i * vec->ele_size);
memcpy(dest, your_data, vec->ele_size);
```

---

## 3. 核心难点：分层所有权 (Layered Ownership)

这是本章最重要的部分，也是 C 语言容器与 C++ STL 最大的区别。

**问：当我调用 `vector_free(vec)` 时，Vector 里的元素会被自动释放吗？**

**答：这取决于你存的是什么。** Vector 采用的是 **“浅层管理”** 策略。

### 场景 A：存值 (Value Semantics) —— 安全

假设你存的是 `int` 或者 `Point {x, y}` 这种纯数据结构体。

```c
// vector_new(sizeof(int));
Vector 内存布局: [ 10 ] [ 20 ] [ 30 ] ...
```

当你调用 `vector_free` 时，Vector 释放了存放 `[10, 20, 30]` 的这块内存。**一切都很完美，没有泄漏。**

### 场景 B：存指针 (Pointer Semantics) —— 危险！

假设你要存 `Token`。为了避免拷贝大的结构体，你决定存 `Token*`（指针）。

```c
// vector_new(sizeof(Token*));
// 存入的是指针，占 8 字节
Vector 内存布局: [ Ptr1 ] [ Ptr2 ] [ Ptr3 ] ...
                    |        |        |
                    v        v        v
                 {Token}  {Token}  {Token}  <-- 散落在堆内存某处
```

当你调用 `vector_free` 时：
1.  Vector 释放了 `data` 数组（即销毁了 `[Ptr1, Ptr2, Ptr3]` 这张表）。
2.  **但是！** `Ptr1` 指向的那块 `{Token}` 内存 **并没有被释放**。

**结果**：Vector 销毁了，但 Token 变成了 **孤儿内存（泄漏）**。

### 我们的设计原则：容器只管容器，元素只管元素

为了保持 Vector 的通用性（Vector 根本不知道它存的指针指向哪里，也不知道该怎么释放它），我们必须遵守以下 **所有权契约**：

1.  **Vector 的职责**：只负责维护 `data` 数组本身的内存（扩容、释放数组）。
2.  **用户的职责**：如果你在 Vector 里存了指针，**在销毁 Vector 之前，你必须手动遍历并释放每一个元素。**

#### 正确的使用范式：

```c
// 1. 创建存指针的 Vector
Vector *v = vector_new(sizeof(Token*));

// 2. 存入指针
Token *t = malloc(sizeof(Token));
vector_push_back(v, &t); // 注意：这里传入的是指针的地址（&t），即 T**

// ... 使用 ...

// 3. 【关键】退出前，手动释放元素
for (size_t i = 0; i < v->size; i++) {
    // 拿出来的是 Token**，需要解引用一次变成 Token*
    Token **ptr = (Token**)vector_get(v, i);
    free(*ptr); // 释放具体的 Token 内存
}

// 4. 最后释放 Vector 外壳
vector_free(v);
```

---

## 4. 接口契约 (API Design)

理解了所有权模型后，我们来看一下 `include/vector.h` 的设计。
请仔细阅读代码注释，它们定义了我们这个库的使用规则。

```c
#pragma once
#include <stddef.h>

typedef struct Vector Vector;

// 对外隐藏具体实现，只暴露指针，
// 但在我们的教程中，为了教学方便，我们在 vector.c 中定义完整结构，
// 或者你可以把 struct 定义放在 .h 里以便内联优化（我们在本教程中选择放在 .h）。
struct Vector {
    void *data;
    size_t size;
    size_t capacity;
    size_t ele_size;
};

// 创建 Vector
// ele_size: 比如 sizeof(int) 或 sizeof(Token*)
Vector *vector_new(size_t ele_size);

// 释放 Vector
// 注意：这只是销毁容器本身。如果容器里存的是指针，请先手动释放元素内容！
void vector_free(Vector *vec);

// 压入元素
// elem: 这是一个 void*，指向你要存的数据的地址
// 比如 int a = 10; vector_push_back(v, &a);
int vector_push_back(Vector *vec, void *elem);

// 弹出尾部元素
// 注意：这仅仅是把 size 减一。如果存的是指针，该指针指向的内存不会被 free。
int vector_pop_back(Vector *vec);

// 获取元素
// 返回的是指向 Vector 内部数据的指针。
// 用户拿到后通常需要强转： *(int*)vector_get(v, 0)
void *vector_get(Vector *vec, size_t idx);

// 辅助函数
void *vector_front(Vector *vec);
void *vector_back(Vector *vec);
```

---

## 5. 扩容策略：为什么是“翻倍”？

你会在 `vector.c` 中看到这样的扩容逻辑：

```c
size_t new_cap = vec->capacity ? vec->capacity * 2 : 4;
```

**为什么是 `* 2` 而不是 `+ 1`？**

-   **+ 1 策略**：每插入一个元素都要 `realloc`。`realloc` 涉及内存分配和可能的**全量数据搬运**。插入 N 个元素，需要搬运 $1+2+...+N = O(N^2)$ 次。效率极低。
-   **x 2 策略**：虽然每次搬运量大，但搬运次数极少（对数级别）。平均分摊到每次 `push` 操作上，时间复杂度是 **$O(1)$**。

这是所有现代动态数组（Java ArrayList, C++ std::vector, Python list）的标准做法。

---

### 下一章预告

理论讲完了，所有权模型也明确了。

下一章 **[06. 动态数组：代码实现](06_vector_impl.md)**，我们将把提供的 `vector.c` 源码放入项目中，并对其中几个精妙的实现细节（如 `memcpy` 的使用、切片 `slice` 的实现）进行代码走读。