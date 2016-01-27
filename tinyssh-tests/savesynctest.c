/*
20140417
Jan Mojzis
Public domain.
*/

#include "savesync.h"
#include "byte.h"
#include "fail.h"
#include "load.h"
#include "randombytes.h"
#include "writeall.h"
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define SPACELEN 1048577

unsigned char space1[SPACELEN];
unsigned char space2[SPACELEN];

static void test1(void)
{

    randombytes(space1, sizeof space1);

    if (savesync("savesynctest.data", space1, sizeof space1) == -1)
        fail("savesync() failure");
    if (load("savesynctest.data", space2, sizeof space2) == -1)
        fail("load() failure");
    if (!byte_isequal(space1, SPACELEN, space2))
        fail("load()/savesync() failure");
}

int main(void)
{
    test1();
    _exit(0);
}
