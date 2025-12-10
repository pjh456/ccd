# 04. 驯服内存巨兽：栈、堆与动态内存管理

**本章目标**：

1. **核心痛点**：为什么 `int arr[100];` 这种写法在工程中往往是不合格的？
2. **概念区分**：彻底搞懂 **栈 (Stack)** 和 **堆 (Heap)** 的区别。
3. **四大金刚**：熟练掌握 `malloc`、`free`、`calloc` 和 **`realloc`**。
4. **避坑指南**：如何避免 C 语言新手最容易犯的“段错误”和“内存泄漏”。

---

## 1. 数组的尴尬：为什么我们需要动态内存？

在 C 语言课本里，我们通常这样定义数组：
```c
// 定义一个能存 100 个整数的数组
int scores[100];
```

但这在实际工程（比如我们的查重器）中有一个巨大的致命伤：**死板**。

- **场景 A**：你只想读 5 个数。结果你申请了 100 个空间，浪费了 95 个。
- **场景 B**：你需要读入一个 10 万行的源代码文件。结果数组只有 100，程序直接崩溃（溢出）。

你可能会说：“那我定义 `int scores[1000000];` 不就行了？”
**不行！** 这种数组是存放在**栈 (Stack)** 上的。栈的空间非常小（通常只有几 MB）。如果你敢在函数里写 `int arr[10000000];`，程序运行瞬间就会因为 **Stack Overflow (栈溢出)** 而暴毙。

我们需要一种机制，能够 **“按需索取，用完即还”**，而且空间巨大。这就是 **堆 (Heap)**。

---

## 2. 内存的双城记：栈 vs 堆

我们可以把内存想象成两家旅馆：

### 栈 (Stack)：快捷胶囊旅馆

- **特点**：自动管理，速度极快，但空间狭小。
- **谁住这里**：局部变量（`int a`, `float b`）、函数参数。
- **生命周期**：**函数结束，自动退房**。数据瞬间消失。

### 堆 (Heap)：豪华大仓库

- **特点**：手动管理，空间巨大（取决于你的内存条），速度稍慢。
- **谁住这里**：`malloc` 申请的数据。
- **生命周期**：**你不主动释放 (`free`)，它就永远在那里**（直到程序结束）。

---

## 3. 内存管理的“四大金刚”

要使用堆内存，我们需要引入标准库头文件：
```c
#include <stdlib.h>
```

### `malloc` (Memory Allocate) —— 给我一块地

```c
// 申请能放 10 个 int 的空间
// sizeof(int) 通常是 4 字节，所以这里申请了 40 字节
int *arr = (int*)malloc(10 * sizeof(int));

// 必做检查：如果内存耗尽，malloc 会返回 NULL
if (arr == NULL) {
    // 报错并退出
}
```

### `free` —— 退房走人

C 语言没有垃圾回收（GC）。

如果你申请了内存却不释放，这就叫 **内存泄漏 (Memory Leak)**。

久而久之，电脑内存就会被吃光。
```c
free(arr); // 归还内存
arr = NULL; // 养成好习惯：释放后将指针置空，防止“野指针”
```

### `calloc` (Clean Allocate) —— 拎包入住

`malloc` 申请的内存里是乱码（旧数据）。

`calloc` 会多做一步：**把内存全部清零**。
```c
// 申请 10 个 int，并全部初始化为 0
int *arr = (int*)calloc(10, sizeof(int));
```

### `realloc` (Re-Allocate) —— 扩建魔法

这是最神奇的一个函数，也是我们在下一章写 Vector 的核心。

**场景**：你申请了 10 个空间，结果发现不够用了，想要 20 个。
```c
// 尝试把 arr 的空间扩大到 20 个 int
// 原来的数据（前 10 个）会被保留！
int *new_arr = (int*)realloc(arr, 20 * sizeof(int));

if (new_arr != NULL) {
    arr = new_arr; // 更新指针
}
```
> **注意**：`realloc` 可能会把你的数据搬家到内存的另一块空地，所以 **必须** 更新指针变量 `arr`。

---

## 4. 常见的“三种死法”

新手写 C 语言，90% 的崩溃都源于以下三个错误：

### 死法一：段错误 (Segmentation Fault)

试图访问不属于你的内存。
```c
int *p = NULL;
*p = 10; // 崩溃！不能往空地址写数据。

int *arr = malloc(10);
free(arr);
arr[0] = 5; // 崩溃！试图访问已经退房的内存 (Use After Free)。
```

### 死法二：内存泄漏 (Memory Leak)

光借不还。
```c
void loop() {
    while(1) {
        int *p = malloc(1024); 
        // 忘记 free(p) 了！
    }
}
// 几秒钟后，你的电脑内存就被吃光卡死了。
```

### 死法三：野指针 (Wild Pointer)

指针指向了一块随机的垃圾地址。
```c
int *p; // 没初始化，p 的值是随机的
*p = 100; // 可能会崩溃，也可能修改了关键系统数据导致更诡异的 bug。
```

---

## 5. 实战演练：动态数组

让我们在 `src` 目录下新建一个 `memory_demo.c`（这只是个练习，不在最终项目中），体验一下动态扩容的魅力。
```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    int size = 2; // 初始只给 2 个位置
    int count = 0;
    
    // 1. 初始申请
    int *numbers = (int*)malloc(size * sizeof(int));
    if (!numbers) return 1;

    printf("Input integers (input -1 to stop):\n");

    while (1) {
        int input;
        scanf("%d", &input);
        if (input == -1) break;

        // 2. 如果满了，触发扩容机制
        if (count == size) {
            size *= 2; // 容量翻倍
            printf(">> Expanding capacity to %d...\n", size);
            
            // 重新分配内存
            int *new_numbers = (int*)realloc(numbers, size * sizeof(int));
            if (!new_numbers) {
                printf("Out of memory!\n");
                free(numbers); // 别忘了释放旧的
                return 1;
            }
            numbers = new_numbers;
        }

        numbers[count++] = input;
    }

    // 3. 打印结果
    printf("You entered: ");
    for (int i = 0; i < count; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");

    // 4. 清理现场
    free(numbers);
    numbers = NULL;

    return 0;
}
```

**编译与运行**

由于我们用的是 CMake，如果你想编译这个临时文件，

可以直接用 gcc 命令（省去改 CMakeLists 的麻烦）：
```bash
gcc src/memory_demo.c -o demo
./demo
```

**体验一下**

当你输入第 3 个数时，程序会自动提示扩容。

这就是 `std::vector` 和我们即将实现的 `Vector` 背后的原理！

---

## 6. 进阶概念：`void*` 通用指针

在下一章中，我们要写一个通用的容器。

- 如果我们写 `int*`，它只能存整数。
- 如果我们写 `Token*`，它只能存 Token。

C 语言提供了一个神器：**`void*`**。

- 它可以指向 **任何类型** 的数据。
- `void*` 就像是一个没有标签的快递盒。你知道里面有东西，但不知道是什么。
- 使用时，需要把它 **强制转换** 回原本的类型（比如 `(int*)ptr`）。

---

### 下一章预告

掌握了 `malloc`、`realloc` 和 `void*`，你就集齐了打造神兵利器的所有材料。

下一章 **[05. 动态数组：设计思路](05_vector_theory.md)**，我们将不使用 C++ 的 `vector`，而是亲手用 C 语言打造一个功能完全相同的 **泛型动态数组 (Vector)**，它是整个查重器项目的基石。