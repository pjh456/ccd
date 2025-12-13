#pragma once

#include <stddef.h>

typedef struct Vector Vector;
typedef enum CompileStage CompileStage;
typedef struct CompileOptions CompileOptions;

enum CompileStage
{
    STAGE_TOKENS, // 仅词法分析
    STAGE_UNITS,  // 粗粒度语句单元
    STAGE_AST,    // AST（未来）
    STAGE_IR,     // IR（未来）
};

struct CompileOptions
{
    const char *input;
    CompileStage stage;
};

void parse_args(int argc, char **argv, CompileOptions *opt);

char *read_file(const char *path, size_t *out_len);

Vector *load_and_tokenize(const char *path);

void dump_tokens(Vector *tokens);

void dump_units(Vector *tokens);