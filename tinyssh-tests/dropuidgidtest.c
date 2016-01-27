/*
20140320
Jan Mojzis
Public domain.
*/

#include "dropuidgid.h"
#include "fail.h"
#include "run.h"
#include <unistd.h>

static void droproot(void)
{

    if (geteuid() == 0) {
        if (!dropuidgid(123000, 123000))
            _exit(111);
    }
    if (geteuid() == 0)
        _exit(111);
}

static void test1(void)
{
    droproot();
    if (dropuidgid(123001, getegid()))
        _exit(111);
    _exit(0);
}

static void test2(void)
{
    droproot();
    if (!dropuidgid(geteuid(), getegid()))
        _exit(111);
    if (!dropuidgid(geteuid(), getegid()))
        _exit(111);
    _exit(0);
}

static void test3(void)
{
    droproot();
    if (!dropuidgid(0, 0))
        _exit(111);
    _exit(0);
}

int main(void)
{

    run_mustpass(test1);
    run_mustpass(test2);
    run_mustfail(test3);
    _exit(0);
}
