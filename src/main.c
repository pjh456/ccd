#include <stdio.h>

#include "ccd_cli.h"

int main(int argc, char **argv)
{
    CompileOptions opt;
    parse_args(argc, argv, &opt);

    Vector *tokens = load_and_tokenize(opt.input);

    switch (opt.stage)
    {
    case STAGE_TOKENS:
        dump_tokens(tokens);
        break;

    case STAGE_UNITS:
        dump_units(tokens);
        break;

    case STAGE_AST:
        fprintf(stderr, "AST stage not implemented yet\n");
        break;
    case STAGE_IR:
        fprintf(stderr, "AST stage not implemented yet\n");
        break;
    default:
        break;
    }
    return 0;
}