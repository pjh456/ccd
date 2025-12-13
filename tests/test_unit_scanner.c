#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tokenizer.h"
#include "vector.h"
#include "unit_scanner_impl/statement_unit.h"
#include "unit_scanner.h"

// ===============================
// Auto-scan test
// ===============================

void test_auto_scan()
{
    printf("=== Auto UnitScanner Test ===\n");

    const char *src =
        "{"
        "  int a = 1;"
        "  a++;"
        "  if (a) { return a; } else { goto label; }"
        "  while(1) continue;"
        "  do { break; } while(0);"
        "  for(;;);"
        "  switch(a) { case 1: a=2; default: ; }"
        "  label: ;"
        "}";

    // 1. tokenize
    Vector *tokens = tokenize_all(src);

    // 2. scanner
    UnitScanner *us = unit_scanner_new(tokens);
    assert(us);

    // 3. root compound
    Vector *children = vector_new(sizeof(StatementUnit *));
    assert(children);

    // 跳过 '{'
    StatementUnit *root = scan_file(us);
    assert(root);
    assert(root->type == SUT_COMPOUND);

    // 你可以只打印 root（最理想）
    print_statement_unit(root, 1);

    statement_unit_free(root);
    unit_scanner_free(us);
    // vector_free(tokens);

    printf("=== Auto Scan OK ===\n");
}

int main(void)
{
    test_auto_scan();
    return 0;
}
