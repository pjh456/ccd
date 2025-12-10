#pragma once

#include <stdint.h>

/**
 * @brief 通用动态数组 (Vector)
 * 存储任意类型的元素，类似 C++ 的 std::vector<void*> 变体。
 */
typedef struct Vector
{
    void *data;      ///< 数据区指针
    size_t size;     ///< 当前元素个数
    size_t capacity; ///< 当前分配的容量
    size_t ele_size; ///< 单个元素的大小 (字节)
} Vector;

/**
 * @brief 创建一个新的 Vector
 * @param ele_size 元素的字节大小 (例如 sizeof(int))
 */
Vector *vector_new(size_t ele_size);

/**
 * @brief 释放 Vector 内存
 * @note 不会释放元素内部指针指向的内存，仅释放 vector 自身结构和 data 块。
 */
void vector_free(Vector *vec);

/**
 * @brief 预留空间
 * 确保 vector 至少能容纳 new_cap 个元素，避免频繁 realloc。
 */
int vector_reserve(Vector *vec, size_t new_cap);

/**
 * @brief 向尾部添加元素
 * 如果容量不足，会自动扩容（通常是翻倍）。
 * @param elem 指向要复制的数据的指针
 */
int vector_push_back(Vector *vec, void *elem);

/**
 * @brief 移除尾部元素
 * 仅仅减少 size 计数，不释放内存。
 */
int vector_pop_back(Vector *vec);

/**
 * @brief 获取指定索引的元素
 * @return void* 指向元素的指针，越界返回 NULL
 */
void *vector_get(Vector *vec, size_t idx);

/**
 * @brief 获取首部的元素
 * @return void* 指向元素的指针，越界返回 NULL
 */
void *vector_front(Vector *vec);

/**
 * @brief 获取尾部的元素
 * @return void* 指向元素的指针，越界返回 NULL
 */
void *vector_back(Vector *vec);