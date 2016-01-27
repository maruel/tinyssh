/* Public domain. */
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

int main()
{

    size_t len = 16;
    char* x;

    x = malloc(len);
    if (!x)
        return 1;
    if (mlock(x, len) == -1)
        return 1;
    if (munlock(x, len) == -1)
        return 1;
    printf("/* Public domain. */\n\n");
    printf("#define HASMLOCK 1\n");
    return 0;
}
