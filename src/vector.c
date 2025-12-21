#include "vector.h"

#include <stdlib.h>
#include <string.h>

Vector *vector_new(size_t ele_size)
{
    if (!ele_size)
        return NULL;
    Vector *vec = (Vector *)malloc(sizeof(*vec));

    vec->data = NULL;
    vec->size = 0;
    vec->capacity = 0;
    vec->ele_size = ele_size;

    return vec;
}

void vector_free(Vector *vec)
{
    if (!vec)
        return;
    if (vec->data)
        free(vec->data);
    free(vec);
    vec = NULL;
}

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

int vector_resize(Vector *vec, size_t new_size)
{
    size_t origin_size = vec->size;
    if (!vector_reserve(vec, new_size))
        return 0;
    vec->size = new_size;

    if (vec->size > origin_size)
        memset(
            vec->data + origin_size * vec->ele_size,
            0,
            (vec->size - origin_size) * vec->ele_size);

    return 1;
}

int vector_push_back(Vector *vec, void *elem)
{
    if (!vec)
        return 0;

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

int vector_pop_back(Vector *vec)
{
    if (!vec || vec->size == 0)
        return 0;
    // free(vector_back(vec)); // free() 只能释放首地址的内存
    vec->size--;
    return 1;
}

void *vector_get(Vector *vec, size_t idx)
{
    if (!vec || idx >= vec->size)
        return NULL;
    return (char *)vec->data + idx * vec->ele_size;
}

void *vector_front(Vector *vec) { return vector_get(vec, 0); }

void *vector_back(Vector *vec) { return vector_get(vec, vec->size - 1); }

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
