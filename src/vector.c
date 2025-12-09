#include "vector.h"

#include <stdlib.h>
#include <string.h>

Vector *vector_new(size_t ele_size)
{
    Vector *vec = (Vector *)malloc(sizeof(*vec));
    vec->data = NULL;
    vec->size = 0;
    vec->capacity = 0;
    vec->ele_size = ele_size;
    return vec;
}

void vector_free(Vector *vec)
{
    if (vec->data)
        free(vec->data);
    free(vec);
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

void *vector_get(Vector *vec, size_t idx)
{
    if (idx > vec->size)
        return NULL;
    return (char *)vec->data + idx * vec->ele_size;
}

int vector_pop_back(Vector *vec)
{
    if (vec->size == 0)
        return 0;
    vec->size--;
    return 1;
}