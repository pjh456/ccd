#pragma once

#include <stdint.h>

typedef struct Vector
{
    void *data;
    size_t size;
    size_t capacity;
    size_t ele_size;
} Vector;

Vector *vector_new(size_t ele_size);

void vector_free(Vector *vec);

int vector_reserve(Vector *vec, size_t new_cap);

int vector_push_back(Vector *vec, void *elem);

void *vector_get(Vector *vec, size_t idx);