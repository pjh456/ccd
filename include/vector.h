#pragma once

#include <stdint.h>

typedef struct Vector Vector;

/**
 * @brief 通用动态数组 (Vector)
 * 存储任意类型的元素，类似 C++ 的 std::vector<void*> 变体。
 *
 * @note 在保存类型 T 的指针时，
 * 应当以 sizeof(T*) 作为元素字节大小，
 * 同时在释放 Vector 前手动遍历并释放每个元素。
 * 即 Vector 在此时保存 T** 以避免产生元素拷贝。
 */
struct Vector
{
    void *data;      // 数据区指针
    size_t size;     // 当前元素个数
    size_t capacity; // 当前分配的容量
    size_t ele_size; // 单个元素的大小 (字节)
};

/**
 * @brief 创建一个新的 Vector
 *
 * @param ele_size 元素的字节大小，
 * 如 sizeof(int)
 *
 * @return 返回一个新的 Vector
 *
 * @note 当 ele_size = 0 时属于未定义行为，
 * 此时返回一个 NULL 类型的 Vector 以避免异常。
 */
Vector *vector_new(size_t ele_size);

/**
 * @brief 释放 Vector 内存
 *
 * @param vec 进行操作的 Vector
 *
 * @note 不会释放元素内部指针指向的内存，
 * 仅释放 vector 自身结构和 data 块，
 * 释放完毕后会将原 Vector 指针置空。
 */
void vector_free(Vector *vec);

/**
 * @brief 返回一个新的 Vector 拷贝切片
 *
 * @param vec 进行操作的 Vector
 * @param begin 切片开始的下标
 * @param end 切片结束的下标
 *
 * @return int 成功返回 1，否则返回 0
 *
 * @note 其中元素会进行浅拷贝，
 * 切片的范围为 [begin, end)
 */
Vector *vector_slice(Vector *vec, size_t begin, size_t end);

/**
 * @brief 预留空间，
 * 确保 vector 至少能容纳 new_cap 个元素，
 * 避免频繁扩容。
 *
 * @param vec 进行操作的 Vector
 * @param new_cap 新的扩容个数
 *
 * @return int 成功返回 1，否则返回 0
 *
 * @note 不会进行内存填充，
 * Vector 的 size 也不会改变，
 * 只改变了 capacity。
 */
int vector_reserve(Vector *vec, size_t new_cap);

/**
 * @brief 调整大小，
 * 确保 vector 一定有 new_size 个元素，
 * 避免频繁扩容
 *
 * @param vec 进行操作的 Vector
 * @param new_size 扩容后的元素个数
 *
 * @return int 成功返回 1，否则返回 0
 *
 * @note 扩容产生的新内存字节填充为 0
 */
int vector_resize(Vector *vec, size_t new_size);

/**
 * @brief 向 Vector 尾部添加元素，
 * 如果容量不足，会自动翻倍扩容。
 *
 * @param vec 进行操作的 Vector
 * @param elem 指向要复制的数据的指针
 *
 * @return int 成功返回 1，否则返回 0
 *
 * @note （理论上）允许插入一个空元素。
 */
int vector_push_back(Vector *vec, void *elem);

/**
 * @brief 移除 Vector 尾部元素，
 * 并释放元素内存。
 *
 * @param vec 进行操作的 Vector
 *
 * @return int 成功返回 1，否则返回 0
 *
 * @note 当存入 T* 类型时，
 * 此时根据定义只释放了一个 T** 指针，
 * 因此应当在使用前手动调用 T* 的析构函数。
 */
int vector_pop_back(Vector *vec);

/**
 * @brief 获取 Vector 指定索引的元素
 *
 * @param vec 进行操作的 Vector
 * @param idx 访问的索引下标
 *
 * @return void* 指向元素的容器指针，越界返回 NULL
 *
 * @note 对 resize 扩容出来的元素，
 * 需要注意其填充 0 字节的影响。
 */
void *vector_get(Vector *vec, size_t idx);

/**
 * @brief 获取 Vector 首部的元素
 *
 * @param vec 进行操作的 Vector
 *
 * @return void* 指向元素的指针，为空时返回 NULL
 */
void *vector_front(Vector *vec);

/**
 * @brief 获取 Vector 尾部的元素
 *
 * @param vec 进行操作的 Vector
 *
 * @return void* 指向元素的指针，为空时返回 NULL
 */
void *vector_back(Vector *vec);

/**
 * @brief 向 Vector 指定位置插入元素
 *
 * @param vec 进行操作的 Vector
 * @param idx 要插入的数据下标
 * @param elem 指向要复制的数据的指针
 *
 * @return int 成功返回 1，否则返回 0
 */
int vector_insert(Vector *vec, size_t idx, void *elem);

/**
 * @brief 向 Vector 指定位置删除元素
 *
 * @param vec 进行操作的 Vector
 * @param idx 要移除的数据下标
 *
 * @return int 成功返回 1，否则返回 0
 */
int vector_remove(Vector *vec, size_t idx);