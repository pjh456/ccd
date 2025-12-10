# 05. 拒绝重复造轮子：通用动态数组 (Vector) 的设计哲学

**本章目标**：

1.  **架构思维**：学习如何把“内存管理”封装起来，不再手动写 `realloc`。
2.  **泛型编程**：在 C 语言没有 template（模板）的情况下，如何实现一个既能存 `int` 又能存 `Token` 的容器？
3.  **性能权衡**：为什么扩容时要“容量翻倍”，而不是“加一个位置”？
4.  **接口设计**：先写 `.h`，想好我们需要什么功能。

---

## 1. 为什么我们需要封装？

在上一章的练习中，我们写了这样一段代码来扩容：

```c
if (count == size) {
    size *= 2;
    int *new_arr = realloc(arr, size * sizeof(int));
    // ... 检查 NULL ...
    arr = new_arr;
}
```

这段代码没有任何问题。但问题在于：

- 我们在 Tokenizer 里要存 `Token`，需要写一遍这个逻辑。
- 我们在 Parser 里要存 `ASTNode`，又要写一遍。
- 我们在 Analyzer 里要存 `Hash`，还得写一遍。

**重复代码是万恶之源。** 

如果你把 `realloc` 的逻辑写错了（比如忘了更新 size），所有地方都会炸。

我们需要设计一个 **通用的容器（Vector）**，把它做成一个黑盒子：

- **对外**：你只管往里面 `push` 数据，不用管内存够不够。
- **对内**：它自己负责 `malloc`、`free` 和扩容。

---

## 2. 核心结构设计：Vector 的“三大件”

一个成熟的动态数组，在内存里长什么样？

它至少需要维护三个变量。

我们在 `include` 目录下新建 `vector.h`（暂时先只写结构体定义）：

```c
typedef struct {
    void *data;       // 1. 指向堆内存的指针（仓库的钥匙）
    size_t size;      // 2. 当前存了多少个元素（实际库存）
    size_t capacity;  // 3. 当前分配了多少内存（仓库总容量）
    size_t ele_size;  // 4. 每个元素有多大（单件商品体积）
} Vector;
```

### 字段详解

1. **`data` (void*)**：
    - 上一章我们用了 `int*`，因为它只存整数。
    - 为了通用，我们这里用 **`void*`**。它指向一块“未定义类型”的内存区域。不管你是存 `int` 还是存 `struct Token`，对 CPU 来说都是一堆二进制字节。

2. **`size` vs `capacity`**：
    - **size**：是你实际放进去的数据量。`vec.push(a)` 后，size + 1。
    - **capacity**：是 `malloc` 实际申请的内存量。
    - **核心逻辑**：只有当 `size == capacity`（仓库满了）时，才触发扩容。平日里 `push` 数据只需往空位放，速度极快。

3. **`ele_size` (Element Size)**：
    - 这是 C 语言泛型的关键！
    - 因为 `void*` 没有类型信息，编译器不知道 `data[1]` 该跳过多少字节。
    - 如果你存 `int`，`ele_size` 就是 4。
    - 如果你存 `Token`，`ele_size` 可能是 32。
    - 我们必须把这个信息存下来，后面通过 `memcpy` 来拷贝数据。

---

## 3. 扩容策略：为什么是“翻倍”？

当 `size == capacity` 时，我们需要申请更大的内存。

常见的有两种策略：

### 策略 A：每次加 1 (Arithmetic Growth)

- 满了？申请 `size + 1` 的空间，把旧数据搬过去。
- **后果**：每次 `push` 都要搬家。如果你插入 1000 个元素，就要搬 1000 次家。效率极低（O(n^2)）。

### 策略 B：每次翻倍 (Geometric Growth)

- 满了？申请 `size * 2` 的空间。
- **后果**：
    - 第 1 次搬家（容量 1->2）
    - 第 2 次搬家（容量 2->4）
    - 第 4 次搬家（容量 4->8）
    - ...
- 虽然搬家很累，但搬家的次数非常少（对数级别）。平均下来，每次 `push` 的操作几乎是瞬间完成的（**均摊 O(1)**）。

**结论**：我们将采用 **容量翻倍** 策略。

> 在真实工程项目中，很少会直接使用翻倍扩容策略，
> 而是根据具体情景设置因子为 1.5 等其他数值。
---

## 4. 泛型存储：`memcpy` 的魔法

在 C++ 中，你可以用 `vec[i] = x` 来赋值。

在 C 语言中，因为 `data` 是 `void*`，我们不能直接赋值。

我们需要像搬运工一样，按 **字节** 搬运数据。

```c
// 假设我们要把变量 x 存入 vector 的第 i 个位置
// data 是仓库起始地址
// i * ele_size 是偏移量（跳过前 i 个元素）
void *dest = (char*)vector->data + (i * vector->ele_size);

// 从 x 的地址，拷贝 ele_size 这么多字节，到 dest
memcpy(dest, &x, vector->ele_size);
```

这就是 C 语言实现泛型的本质：

**我不关心你是什么类型，我只把你的内存比特流拷贝一份保存起来。**

---

## 5. 接口定义 (API Design)

现在，让我们把设计落实到代码。请完善 `include/vector.h`：
```c
#pragma once

#include <stddef.h> // for size_t

typedef struct {
    void *data;
    size_t size;
    size_t capacity;
    size_t ele_size; // 核心：每个元素占多少字节
} Vector;

// 1. 创建与销毁
// 创建时必须告诉 Vector 你要存什么类型的数据（的大小）
// 比如：vector_new(sizeof(int))
Vector *vector_new(size_t ele_size);

void vector_free(Vector *vec);

// 2. 增删改查
// 这里的 elem 是 void*，意味着你可以传 &my_int, &my_token
int vector_push_back(Vector *vec, void *elem);

// 获取第 idx 个元素的指针
// 返回 void*，用户拿到后需要自己强转，比如 *(int*)ptr
void *vector_get(Vector *vec, size_t idx);

// 3. 辅助功能
size_t vector_size(Vector *vec);
int vector_empty(Vector *vec);
```

---

## 常见问题 (FAQ)

**Q: 为什么不直接用 C++？C++ 自带 `std::vector` 啊。**
A: 问得好。如果这是生产环境，我绝对推荐用 C++。但这是 **学习**。
只有亲手用 C 语言造过 Vector，你才能深刻理解 C++ 里的“构造函数”、“析构函数”、“拷贝构造”是在什么时候被调用的，以及为什么 `vector` 扩容会导致迭代器失效。**我们在造轮子，是为了懂轮子。**

**Q: `vector_get` 返回的是指针，如果我修改这个指针指向的内容，Vector 里的数据会变吗？**
A: 会！这正是我们想要的。
`void *p = vector_get(v, 0);` 拿到的是 Vector 内部内存的 **直接引用**。
如果你写 `*(int*)p = 999;`，那么 Vector 内部存储的数据就被改成了 999。

---

### 下一章预告

设计图已经画好了（`vector.h`）。

下一章 **[06. 动态数组：代码实现](06_vector_impl.md)**，我们将进入 `src/vector.c`，真刀真枪地实现 `malloc`, `realloc`, `memcpy` 的组合拳，亲手造出我们的第一个通用容器！