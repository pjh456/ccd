#include <tokenizer.h>
#include <ctype.h>

#define ONCE(block) \
    do              \
    {               \
        block       \
    } while (0)

int is_alpha(unsigned char ch) { return isalpha(ch) || ch == '_'; }
int is_digit(unsigned char ch) { return ch >= '0' && ch <= '9'; }
int is_alnum(unsigned char ch) { return is_alpha(ch) || is_digit(ch); }
int is_space(unsigned char ch) { return ch == ' ' || ch == '\t' || ch == '\n' || ch == 'r'; }

#ifdef ENABLE_DEBUG
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG(...)
#endif