#include "osfreq.c"
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

static double cpufrequency = 0;

static void init(void)
{
    cpufrequency = osfreq();
}

long long cpucycles_gettimeofday(void)
{
    double result;
    struct timeval t;
    if (!cpufrequency)
        init();
    gettimeofday(&t, (struct timezone*)0);
    result = t.tv_usec;
    result *= 0.000001;
    result += (double)t.tv_sec;
    result *= cpufrequency;
    return result;
}

long long cpucycles_gettimeofday_persecond(void)
{
    if (!cpufrequency)
        init();
    return cpufrequency;
}
