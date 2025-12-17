#pragma once

#include <stdint.h>

typedef struct Vector Vector;
typedef struct HashEntry HashEntry;
typedef struct HashMap HashMap;

struct HashEntry
{
    char *key;
    void *value;
    struct HashEntry *next;
};

struct HashMap
{
    Vector *hashs;
};

uint32_t hash_str(const char *str);

HashMap *make_hash_map(size_t count);
void hash_map_free(HashMap *map);

HashEntry *make_hash_entry(HashEntry *next, const char *key, void *val);
void hash_entry_free(HashEntry *entry);

HashEntry *hash_map_find(HashMap *map, const char *key);
HashEntry *hash_entry_find(HashEntry *entry, const char *key);

HashEntry *hash_map_insert(HashMap *map, const char *key, void *val);