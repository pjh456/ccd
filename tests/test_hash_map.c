#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "hash_map.h" // 你的头文件

/* ---------- 测试辅助 ---------- */

static void test_hash_stability(void)
{
    printf("[TEST] hash_str stability...\n");

    uint32_t h1 = hash_str("hello");
    uint32_t h2 = hash_str("hello");
    uint32_t h3 = hash_str("world");

    assert(h1 == h2);
    assert(h1 != h3);

    printf("  OK\n");
}

static void test_basic_insert_find(void)
{
    printf("[TEST] basic insert & find...\n");

    HashMap *map = make_hash_map(16);

    int v1 = 42;
    int v2 = 100;

    hash_map_insert(map, "a", &v1);
    hash_map_insert(map, "b", &v2);

    HashEntry *e1 = hash_map_find(map, "a");
    HashEntry *e2 = hash_map_find(map, "b");
    HashEntry *e3 = hash_map_find(map, "c");

    assert(e1 && strcmp(e1->key, "a") == 0);
    assert(e1->value == &v1);

    assert(e2 && strcmp(e2->key, "b") == 0);
    assert(e2->value == &v2);

    assert(e3 == NULL);

    hash_map_free(map);
    printf("  OK\n");
}

/* 强制制造 hash 冲突：小桶数 */
static void test_collision_chain(void)
{
    printf("[TEST] hash collision chain...\n");

    HashMap *map = make_hash_map(1); // 所有 key 进同一个桶

    int v1 = 1, v2 = 2, v3 = 3;

    hash_map_insert(map, "key1", &v1);
    hash_map_insert(map, "key2", &v2);
    hash_map_insert(map, "key3", &v3);

    HashEntry *e1 = hash_map_find(map, "key1");
    HashEntry *e2 = hash_map_find(map, "key2");
    HashEntry *e3 = hash_map_find(map, "key3");

    assert(e1 && e1->value == &v1);
    assert(e2 && e2->value == &v2);
    assert(e3 && e3->value == &v3);

    hash_map_free(map);
    printf("  OK\n");
}

static void test_duplicate_key(void)
{
    printf("[TEST] duplicate key insert...\n");

    HashMap *map = make_hash_map(8);

    int v1 = 10;
    int v2 = 20;

    hash_map_insert(map, "dup", &v1);
    hash_map_insert(map, "dup", &v2);

    HashEntry *e = hash_map_find(map, "dup");

    assert(e != NULL);
    assert(strcmp(e->key, "dup") == 0);

    assert(
        e->value == &v1 ||
        e->value == &v2);

    hash_map_free(map);
    printf("  OK\n");
}

static void test_empty_key(void)
{
    printf("[TEST] empty string key...\n");

    HashMap *map = make_hash_map(4);

    int v = 77;
    hash_map_insert(map, "", &v);

    HashEntry *e = hash_map_find(map, "");
    assert(e != NULL);
    assert(strcmp(e->key, "") == 0);
    assert(e->value == &v);

    hash_map_free(map);
    printf("  OK\n");
}

static void test_null_key_behavior(void)
{
    printf("[TEST] NULL key behavior...\n");

    HashMap *map = make_hash_map(4);

    /* 期望程序不崩（你可以改成 assert 触发） */
    HashEntry *e = hash_map_find(map, NULL);
    assert(e == NULL);

    hash_map_free(map);
    printf("  OK (or intentionally asserted)\n");
}

/* ---------- 总入口 ---------- */

void test_hash_map_all(void)
{
    printf("==== HashMap Test Begin ====\n");

    test_hash_stability();
    test_basic_insert_find();
    test_collision_chain();
    test_duplicate_key();
    test_empty_key();
    test_null_key_behavior();

    printf("==== HashMap Test All Passed ====\n");
}

int main()
{
    test_hash_map_all();
}