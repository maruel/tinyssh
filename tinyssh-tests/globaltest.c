/*
20140304
Jan Mojzis
Public domain.
*/

#include "global.h"
#include <unistd.h>

#ifndef GLOBAL_BSIZE
error !
#endif

    int
    main(void)
{
    global_init();
    global_init();
    global_purge();
    global_purge();
    global_init();
    global_init();
    _exit(0);
}
