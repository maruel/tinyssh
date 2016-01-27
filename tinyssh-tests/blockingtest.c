/*
20140302
Jan Mojzis
Public domain.
*/

#include "blocking.h"
#include "fail.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{

    int pi[2];
    long long i;

    if (pipe(pi) == -1)
        fail("failure");
    for (i = 0; i < 2; ++i) {
        if ((fcntl(pi[i], F_GETFL) & O_NONBLOCK) != 0)
            fail("failure");
        blocking_disable(pi[i]);
        if ((fcntl(pi[i], F_GETFL) & O_NONBLOCK) == 0)
            fail("failure");
        blocking_enable(pi[i]);
        if ((fcntl(pi[i], F_GETFL) & O_NONBLOCK) != 0)
            fail("failure");
    }
    _exit(0);
}
