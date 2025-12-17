#include "vector.h"

#include <stdlib.h>
#include <string.h>

/**
 * @brief 创建一个新的 Vector
 * @param ele_size 元素的字节大小 (例如 sizeof(int))
 */
Vector *vector_new(size_t ele_size)
{
    Vector *vec = (Vector *)malloc(sizeof(*vec));
    vec->data = NULL;
    vec->size = 0;
    vec->capacity = 0;
    vec->ele_size = ele_size;
    return vec;
}

/**
 * @brief 释放 Vector 内存
 * @note 不会释放元素内部指针指向的内存，仅释放 vector 自身结构和 data 块。
 */
void vector_free(Vector *vec)
{
    if (!vec)
        return;
    if (vec->data)
        free(vec->data);
    free(vec);
}

/**
 * @brief 返回一个新的 Vector
 * @note 其中元素会进行浅拷贝
 */
Vector *vector_slice(Vector *vec, size_t begin, size_t end)
{
    if (!vec || end > vec->size || begin >= end)
        return NULL;

    Vector *out = vector_new(vec->ele_size);
    if (!out)
        return NULL;

    size_t count = end - begin;
    if (!vector_reserve(out, count))
    {
        vector_free(out);
        return NULL;
    }

    memcpy(
        out->data,
        (char *)vec->data + begin * vec->ele_size,
        count * vec->ele_size);
    out->size = count;
    return out;
}

/**
 * @brief 预留空间
 * 确保 vector 至少能容纳 new_cap 个元素，避免频繁 realloc。
 */
int vector_reserve(Vector *vec, size_t new_cap)
{
    if (new_cap <= vec->capacity)
        return 1;

    void *new_data = realloc(vec->data, new_cap * vec->ele_size);
    if (!new_data)
        return 0;

    vec->data = new_data;
    vec->capacity = new_cap;
    return 1;
}

/**
 * @brief 调整大小
 * 确保 vector 有 new_size 个元素，避免频繁 realloc。
 */
int vector_resize(Vector *vec, size_t new_size)
{
    if (!vector_reserve(vec, new_size))
        return 0;
    vec->size = new_size;
    return 1;
}

/**
 * @brief 向尾部添加元素
 * 如果容量不足，会自动扩容（通常是翻倍）。
 * @param elem 指向要复制的数据的指针
 */
int vector_push_back(Vector *vec, void *elem)
{
    if (vec->size == vec->capacity)
    {
        size_t new_cap = vec->capacity ? vec->capacity * 2 : 4;
        if (!vector_reserve(vec, new_cap))
            return 0;
    }
    memcpy(
        (char *)vec->data + vec->size * vec->ele_size,
        elem, vec->ele_size);
    vec->size++;
    return 1;
}

/**
 * @brief 移除尾部元素
 * 仅仅减少 size 计数，不释放内存。
 */
int vector_pop_back(Vector *vec)
{
    if (vec->size == 0)
        return 0;
    vec->size--;
    return 1;
}

/**
 * @brief 获取指定索引的元素
 * @return void* 指向元素的指针，越界返回 NULL
 */
void *vector_get(Vector *vec, size_t idx)
{
    if (idx >= vec->size)
        return NULL;
    return (char *)vec->data + idx * vec->ele_size;
}

/**
 * @brief 获取首部的元素
 * @return void* 指向元素的指针，越界返回 NULL
 */
void *vector_front(Vector *vec) { return vector_get(vec, 0); }

/**
 * @brief 获取尾部的元素
 * @return void* 指向元素的指针，越界返回 NULL
 */
void *vector_back(Vector *vec) { return vector_get(vec, vec->size - 1); }

/**
 * @brief 向指定位置插入元素
 * @param idx 要插入的数据下标
 * @param elem 指向要复制的数据的指针
 */
int vector_insert(Vector *vec, size_t idx, void *elem)
{
    if (!vec || idx > vec->size)
        return 0;

    if (vec->size == vec->capacity)
    {
        size_t new_cap = vec->capacity ? vec->capacity * 2 : 4;
        if (!vector_reserve(vec, new_cap))
            return 0;
    }

    memmove(
        (char *)vec->data + (idx + 1) * vec->ele_size,
        (char *)vec->data + idx * vec->ele_size,
        (vec->size - idx) * vec->ele_size);

    memcpy(
        (char *)vec->data + idx * vec->ele_size,
        elem, vec->ele_size);
    vec->size++;

    return 1;
}

/**
 * @brief 向指定位置删除元素
 * @param idx 要移除的数据下标
 */
int vector_remove(Vector *vec, size_t idx)
{
    if (!vec || idx >= vec->size)
        return 0;

    memmove(
        (char *)vec->data + idx * vec->ele_size,
        (char *)vec->data + (idx + 1) * vec->ele_size,
        (vec->size - idx - 1) * vec->ele_size);
    vec->size--;

    return 1;
}
