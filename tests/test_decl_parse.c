#include "tokenizer.h"
#include "unit_scanner.h"
#include "unit_scanner_impl/statement_unit.h"
#include "decl_parser.h"
#include "decl_parser_impl/decl_unit.h"
#include "vector.h"
#include <stdlib.h>
#include <stdio.h>

// 辅助函数：运行完整的解析流水线
void run_test_pipeline(const char *test_name, const char *src)
{
    printf("========================================\n");
    printf("[Test Case]: %s\n", test_name);
    printf("[Source]:\n%s\n", src);
    printf("----------------------------------------\n");

    // 1. Tokenizer: 源码 -> Token 列表
    Vector *tokens = tokenize_all(src);
    if (!tokens)
    {
        fprintf(stderr, "Fatal: Tokenization failed.\n");
        return;
    }

    // 2. UnitScanner: Token 列表 -> 语句单元 (StatementUnits)
    UnitScanner *us = unit_scanner_new(tokens);
    StatementUnit *root_unit = scan_file(us);

    if (!root_unit || root_unit->type != SUT_COMPOUND)
    {
        fprintf(stderr, "Fatal: Unit scan failed or root is not compound.\n");
        unit_scanner_free(us);
        return;
    }

    // scan_file 返回的是一个包装了所有顶层语句的 Compound Unit
    // 我们需要把里面的 units 拿出来传给 DeclParser
    // 注意：这里的 vector 所有权管理需要小心，DeclParser 会接管这个 vector
    Vector *stmts = root_unit->compound_stmt.units;

    // 3. DeclParser: 语句单元 -> 声明单元 (DeclUnits)
    DeclParser *dp = decl_parser_new(stmts);
    Vector *decl_units = parse_file_decl(dp);

    // 4. 打印结果
    printf("[Parsed DeclUnits]:\n");
    if (decl_units)
    {
        for (size_t i = 0; i < decl_units->size; i++)
        {
            DeclUnit *du = *((DeclUnit **)vector_get(decl_units, i));

            printf("--- Unit #%zu ---\n", i);
            print_decl_unit(du);
        }
    }
    else
        printf("No declaration units parsed (NULL).\n");

    // 5. 清理内存
    decl_parser_free(dp);

    // 释放 root_unit 本身（不包含 children，因为 children 被 parser 释放了）
    free(root_unit);

    // 释放 scanner
    unit_scanner_free(us);

    printf("========================================\n\n");
}

int main(void)
{
    // === Case 1: 基础类型与修饰符 ===
    run_test_pipeline(
        "Basic Types & Qualifiers",
        "int a;"
        "const char b;"
        "extern volatile float c;"
        "unsigned long long d;");

    // === Case 2: 指针 (验证你修复的逻辑) ===
    run_test_pipeline(
        "Pointers",
        "int *p;"
        "char **pp;"
        "void * const cp;"           // 指针本身是 const
        "const int * volatile cipv;" // 指向 const int 的 volatile 指针
    );

    // === Case 3: 数组 ===
    run_test_pipeline(
        "Arrays",
        "int arr[10];"
        "char buf[];"         // 不定长数组
        "float matrix[4][4];" // 多维数组
    );

    // === Case 4: 混合声明 (Declarator List) ===
    run_test_pipeline(
        "Mixed Declarators",
        "int a, *b, c[5];" // 一行内声明 整数、指针、数组
    );

    // === Case 5: 函数原型 ===
    run_test_pipeline(
        "Function Prototypes",
        "void func_void();"
        "int add(int a, int b);"
        "char *strcpy(char *dst, const char *src);"
        "int printf(const char *fmt, ...);" // 变长参数
        "void (*callback)(int);"            // 函数指针
    );

    // === Case 6: 初始化器 (验证嵌套逗号修复) ===
    // 注意：这里我们测试简单的赋值和数组初始化列表
    run_test_pipeline(
        "Initializers",
        "int val = 123;"
        "int expr = 1 + 2 * 3;"
        "int arr[] = {1, 2, 3};"            // 简单列表
        "int mat[2][2] = {{1, 2}, {3, 4}};" // 嵌套列表
    );

    // === Case 7: 非声明语句 ===
    // DeclParser 应该将这些识别为 DUT_EXPRESSION 或 DUT_STATEMENT
    run_test_pipeline(
        "Statements & Expressions",
        "a = 10;"            // 表达式语句
        "if (a) { return; }" // 控制流语句
        "int x;"             // 声明语句混合在其中
        "func();"            // 函数调用
    );

    return 0;
}