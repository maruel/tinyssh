/*
20140303
Jan Mojzis
Public domain.
*/

#include "trymlock.h"
#include <unistd.h>

unsigned char buf[100];

int main(void)
{

    long long i;

    for (i = 81; i < 100; ++i)
        trymlock(buf, i);
    _exit(0);
}
