#include "ccd_cli.h"
#include "tokenizer.h"
#include "unit_scanner.h"
#include "unit_scanner_impl/statement_unit.h"
#include "vector.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parse_args(int argc, char **argv, CompileOptions *opt)
{
    opt->stage = STAGE_TOKENS; // 默认行为你可以自己定
    opt->input = NULL;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-E") == 0)
            opt->stage = STAGE_TOKENS;
        else if (strcmp(argv[i], "-U") == 0)
            opt->stage = STAGE_UNITS;
        else if (strcmp(argv[i], "-A") == 0)
            opt->stage = STAGE_AST;
        else if (argv[i][0] == '-')
        {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            exit(1);
        }
        else
            opt->input = argv[i];
    }

    if (!opt->input)
    {
        fprintf(stderr, "Usage: ccd_cli [-E|-U|-A] file.c\n");
        exit(1);
    }
}

char *read_file(const char *path, size_t *out_len)
{
    FILE *f = fopen(path, "rb");
    if (!f)
    {
        fprintf(stderr, "Error: cannot open file: %s\n", path);
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *buf = malloc((size_t)size + 1);
    if (!buf)
    {
        fprintf(stderr, "Error: out of memory\n");
        exit(1);
    }

    size_t n = fread(buf, 1, size, f);
    fclose(f);

    buf[n] = '\0';
    if (out_len)
        *out_len = n;
    return buf;
}

Vector *load_and_tokenize(const char *path)
{
    size_t len;
    char *src = read_file(path, &len);

    Vector *tokens = tokenize_all(src);
    if (!tokens)
    {
        fprintf(stderr, "Tokenization failed\n");
        exit(1);
    }
    return tokens;
}

void dump_tokens(Vector *tokens)
{
    for (size_t i = 0; i < tokens->size; i++)
    {
        Token *t = vector_get(tokens, i);

        printf("%4d:%-4d  %-12s  \"",
               t->line, t->col, token_name(t->type));
        fwrite(t->str, 1, t->length, stdout);
        printf("\"\n");

        if (t->type == T_EOF)
            break;
    }
}

void dump_units(Vector *tokens)
{
    UnitScanner *us = unit_scanner_new(tokens);
    if (!us)
        return;

    StatementUnit *root = scan_file(us);
    if (root)
    {
        print_statement_unit(root, 0);
        statement_unit_free(root);
    }

    unit_scanner_free(us);
}