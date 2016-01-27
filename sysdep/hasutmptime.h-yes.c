/* Public domain. */
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <utmp.h>

int main(void)
{

    struct utmp ut;

    ut.ut_time = 0;

    printf("#define HASUTMPTIME 1\n");
    return 0;
}
