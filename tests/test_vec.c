#include <stdlib.h>

int main() {
    int *p = malloc(10 * sizeof(int));
    free(p);
    // free(p + 5);

    return 0;
}