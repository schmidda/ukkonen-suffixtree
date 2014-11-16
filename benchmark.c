#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include "benchmark.h"
#ifdef MEMWATCH
#include "memwatch.h"
#endif
/**
 * Get the current memory usage of this process
 * @return the fixed memory in use by this process
 */
long get_mem_usage()
{
    int who= RUSAGE_SELF;
    struct rusage usage;
    struct rusage *p=&usage;

    getrusage(who,p);
    return usage.ru_maxrss;
}
/**
 * Get the current time in microseconds
 * @return the time in microseconds since the epoch
 */
int64_t epoch_time()
{
    struct timeval tv;
    gettimeofday( &tv, NULL );
    return tv.tv_sec*1000000+tv.tv_usec;
}
