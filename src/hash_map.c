#include "hash_map.h"
#include "vector.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

uint32_t hash_str(const char *str)
{
    if (!str)
        return 0;
    uint32_t h = 2166136261u;
    while (*str)
    {
        h ^= (unsigned char)*(str++);
        h *= 16777619u;
    }
    return h;
}

HashMap *make_hash_map(size_t count)
{
    HashMap *map = malloc(sizeof(*map));

    map->hashs = vector_new(sizeof(void *));
    vector_resize(map->hashs, count);

    return map;
}

void hash_map_free(HashMap *map)
{
    if (!map)
        return;
    if (map->hashs)
    {
        for (size_t idx = 0; idx < map->hashs->size; ++idx)
            hash_entry_free(*((HashEntry **)vector_get(map->hashs, idx)));
        vector_free(map->hashs);
    }
    free(map);
}

HashEntry *make_hash_entry(HashEntry *next, const char *key, void *val)
{
    if (!key)
        return NULL;

    HashEntry *entry = malloc(sizeof(*entry));

    entry->key = str_clone(key);
    entry->value = val;
    entry->next = next;

    return entry;
}

void hash_entry_free(HashEntry *entry)
{
    if (!entry)
        return;
    if (entry->key)
        free(entry->key);
    if (entry->value)
        free(entry->value);
    hash_entry_free(entry->next);
    free(entry);
}

HashEntry *hash_map_find(HashMap *map, const char *key)
{
    if (!map || !key || !map->hashs)
        return NULL;

    uint32_t h = hash_str(key);
    size_t idx = h & (map->hashs->size - 1);

    return hash_entry_find(
        *((HashEntry **)vector_get(map->hashs, idx)),
        key);
}

HashEntry *hash_entry_find(HashEntry *entry, const char *key)
{
    if (!entry || !key)
        return NULL;
    for (; entry; entry = entry->next)
        if (strcmp(key, entry->key))
            return entry;
    return NULL;
}

HashEntry *hash_map_insert(HashMap *map, const char *key, void *val)
{
    if (!map || !key || !map->hashs)
        return NULL;

    uint32_t h = hash_str(key);
    size_t idx = h & (map->hashs->size - 1);

    HashEntry *entry = hash_entry_find(
        *((HashEntry **)vector_get(map->hashs, idx)),
        key);

    if (entry)
        return entry;

    HashEntry *new_entry =
        make_hash_entry(
            *((HashEntry **)vector_get(map->hashs, idx)),
            key, val);

    return new_entry;
}