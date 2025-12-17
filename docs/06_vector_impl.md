# 06. 造轮子现场：手写泛型 Vector 实现

**本章目标**：

1.  **代码实现**：将 `src/vector.c` 真正写出来，不再是纸上谈兵。
2.  **核心技法**：
    *   **指针算术**：理解为什么必须用 `(char*)ptr + offset`。
    *   **内存拷贝**：`memcpy` 与 `memmove` 的区别与应用。
3.  **所有权实战**：验证我们的 `vector_free` 是否符合“只管容器，不管元素”的设计契约。

---

## 1. 准备工作

请在 `src/` 目录下新建文件 `vector.c`。

我们需要引入标准库头文件：

```c
#include "vector.h"

#include <stdlib.h> // malloc, free, realloc
#include <string.h> // memcpy, memmove, memset
```

---

## 2. 构造与析构：生命周期的起点与终点

### `vector_new`: 诞生

```c
Vector *vector_new(size_t ele_size) {
    if (!ele_size) return NULL; // 防御性编程

    // 1. 申请【背包本身】的内存
    Vector *vec = (Vector *)malloc(sizeof(Vector));
    if (!vec) return NULL; // 极罕见，内存耗尽

    // 2. 初始化状态
    vec->data = NULL;   // 刚买的背包是空的
    vec->size = 0;      
    vec->capacity = 0;
    vec->ele_size = ele_size; // 记录这只背包是装 int 还是装 Token*

    return vec;
}
```

### `vector_free`: 死亡 (这也是最危险的地方)

请反复阅读这段代码，并牢记我们的**分层所有权**原则。

```c
void vector_free(Vector *vec) {
    if (!vec) return;

    // 1. 释放【数据数组】
    // 这里释放的是 data 指向的那一块连续内存。
    // 如果 data 里存的是 int，没问题。
    // 如果 data 里存的是 Token* (指针)，这里只销毁了指针数组，
    // 指针指向的具体 Token 对象 **依然活着** (如果不手动释放，就是泄漏)。
    if (vec->data) {
        free(vec->data);
    }

    // 2. 释放【背包本身】
    free(vec);
}
```

---

## 3. 内存管理的黑魔法：Reserve & Resize

这两个函数是 Vector 的内功心法，它们不对外暴露太多细节，但负责了所有的肮脏工作（内存分配）。

### `vector_reserve`: 预留空间

它的作用是：**只申请内存，不改变 size**。就像是先把座位占好，但人不坐上去。

```c
int vector_reserve(Vector *vec, size_t new_cap) {
    // 如果现有容量已经够大，就不折腾了
    if (new_cap <= vec->capacity) return 1;

    // realloc 尝试原地扩容，如果原地不够，它会找新地盘并搬家
    // 注意：realloc 计算的是【总字节数】 = 个数 * 单个大小
    void *new_data = realloc(vec->data, new_cap * vec->ele_size);
    
    if (!new_data) return 0; // 内存耗尽

    vec->data = new_data;
    vec->capacity = new_cap; // 更新容量
    return 1;
}
```

### `vector_resize`: 强制调整大小

它的作用是：**既申请内存，又改变 size**。多出来的空间会用 0 填充。

```c
int vector_resize(Vector *vec, size_t new_size) {
    size_t origin_size = vec->size;
    
    // 先确保容量够大
    if (!vector_reserve(vec, new_size)) return 0;
    
    vec->size = new_size;

    // 如果是变大了，把新多出来的区域刷成 0 (避免残留垃圾数据)
    // 指针运算核心：基地址 + (旧终点 * 单个大小)
    if (vec->size > origin_size) {
        memset((char *)vec->data + origin_size * vec->ele_size,
               0,
               (vec->size - origin_size) * vec->ele_size);
    }
    return 1;
}
```

---

## 4. 增删改查：指针算术的艺术

### `vector_push_back`: 压入元素

```c
int vector_push_back(Vector *vec, void *elem) {
    if (!vec) return 0;

    // 1. 检查扩容 (翻倍策略)
    if (vec->size == vec->capacity) {
        size_t new_cap = vec->capacity ? vec->capacity * 2 : 4;
        if (!vector_reserve(vec, new_cap)) return 0;
    }

    // 2. 内存拷贝
    // 目标地址 = data起始 + (size * 单个大小)
    // 必须转为 char* 才能按字节偏移！
    memcpy((char *)vec->data + vec->size * vec->ele_size,
           elem, 
           vec->ele_size);
           
    vec->size++;
    return 1;
}
```

### `vector_insert` & `memmove`

在中间插入元素时，需要把后面的元素整体后移。
这里必须用 `memmove` 而不是 `memcpy`，因为**源区域和目标区域是重叠的**。`memcpy` 处理重叠区域未定义，可能会导致数据错乱。

```c
int vector_insert(Vector *vec, size_t idx, void *elem) {
    // ... 检查扩容逻辑 (同 push_back) ...

    // 把 [idx, end] 的数据整体向后挪一位
    // 源地址：data + idx
    // 目标地址：data + idx + 1
    // 搬运长度：(size - idx) * ele_size
    memmove((char *)vec->data + (idx + 1) * vec->ele_size,
            (char *)vec->data + idx * vec->ele_size,
            (vec->size - idx) * vec->ele_size);

    // 填入新数据
    memcpy((char *)vec->data + idx * vec->ele_size,
           elem, 
           vec->ele_size);
    vec->size++;
    return 1;
}
```

---

## 5. 编译配置更新

Vector 模块完成了。在测试之前，必须告诉 CMake 它的存在。

修改 `CMakeLists.txt`：

```cmake
set(SOURCES
    src/main.c
    src/vector.c  # <--- 新增这行
)
```

---

## 6. 验证时刻：所有权模型测试

让我们编写一段代码，验证我们反复强调的 **“分层所有权”** 是否属实。

修改 `src/main.c`：

```c
#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

// 模拟一个简单的对象
typedef struct {
    int id;
} Object;

int main() {
    printf("--- Testing Pointer Vector Ownership ---\n");

    // 创建一个存 Object* (指针) 的 Vector
    Vector *v = vector_new(sizeof(Object*));

    // 1. 在堆上创建两个对象
    Object *obj1 = malloc(sizeof(Object)); obj1->id = 101;
    Object *obj2 = malloc(sizeof(Object)); obj2->id = 102;

    // 2. 把指针存入 Vector
    // 注意：传入的是指针的地址 &obj1，因为 vector_push_back 内部会做 memcpy
    vector_push_back(v, &obj1);
    vector_push_back(v, &obj2);

    printf("Pushed 2 objects. Vector size: %zu\n", v->size);

    // 3. 正确的释放流程！
    // 必须先手动遍历，释放每一个元素指向的内存
    for (size_t i = 0; i < v->size; i++) {
        Object **ptr = (Object**)vector_get(v, i);
        printf("Freeing object %d at %p\n", (*ptr)->id, (void*)*ptr);
        free(*ptr); // 释放 obj1 和 obj2
    }

    // 4. 最后释放 Vector 容器
    printf("Freeing vector container...\n");
    vector_free(v);

    printf("All clear.\n");
    return 0;
}
```

### 运行并观察

```bash
cd build
cmake .. && cmake --build .
./ccd_cli
```

如果输出如下，说明内存管理逻辑正确：
```text
--- Testing Pointer Vector Ownership ---
Pushed 2 objects. Vector size: 2
Freeing object 101 at 0000021A...
Freeing object 102 at 0000022A...
Freeing vector container...
All clear.
```

如果注释掉第 3 步的 `free(*ptr)`，虽然程序不会报错，但 `Object` 占据的那两块小内存就永远泄露了。

---

## 核心知识点总结

1.  **指针算术 (Pointer Arithmetic)**：`void*` 不能加减。`(char*)data + offset` 是操作原始字节的标准姿势。
2.  **`memmove` vs `memcpy`**：只要涉及同一块内存内部的数据搬运（如插入、删除），**必须** 用 `memmove` 处理重叠。
3.  **所有权契约**：
    *   `vector_free` 负责拆掉“仓库”（释放 `vec->data` 和 `vec`）。
    *   使用者负责处理“货物”（如果是指针，需手动 `free` 指向的对象）。

---

### 下一章预告

有了这把趁手的兵器（Vector），我们终于可以开始处理文件了。

下一章 **[07. 文件读取与缓冲](07_file_io.md)**，我们将编写一个工具函数，演示如何利用标准 I/O 库，高效地将整个源代码文件一次性读入内存，为词法分析做好准备。