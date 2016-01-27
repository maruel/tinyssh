/* Public domain. */
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <utmp.h>

int main(void)
{

    struct utmp ut;

    if (sizeof ut.ut_user < 8)
        return 0;

    printf("#define HASUTMPUSER 1\n");
    return 0;
}
