#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"

static char *read_file(const char *path, size_t *out_len)
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

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s file.c\n", argv[0]);
        return 1;
    }

    size_t len;
    char *src = read_file(argv[1], &len);

    Tokenizer *tk = tokenizer_new(src);

    for (;;)
    {
        Token *t = next(tk);

        printf("%4d:%-4d  %-12s  \"", t->line, t->col, token_name(t->type));

        // 打印 token 原文
        fwrite(t->str, 1, t->length, stdout);
        printf("\"\n");

        if (t->type == T_EOF)
            break;
    }

    tokenizer_free(tk);
    free(src);
    return 0;
}