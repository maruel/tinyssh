/* taken from nacl-20110221, from curvecp/savesync.c */
#include "savesync.h"
#include "open.h"
#include "writeall.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static int writesync(int fd, const void* x, long long xlen)
{
    if (writeall(fd, x, xlen) == -1)
        return -1;
    return fsync(fd);
}

int savesync(const char* fn, const void* x, long long xlen)
{
    int fd;
    int r;
    fd = open_write(fn);
    if (fd == -1)
        return -1;
    r = writesync(fd, x, xlen);
    close(fd);
    return r;
}
