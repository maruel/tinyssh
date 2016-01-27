/* Public domain. */
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <utmp.h>

int main(void)
{

    struct utmp ut;

    printf("#define HASUTMP 1\n");
    return 0;
}
