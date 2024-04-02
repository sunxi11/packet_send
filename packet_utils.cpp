//
// Created by SunX on 2024/4/1.
//

#include <cstdint>
#include <time.h>


uint64_t rdtsc()
{
    unsigned int lo, hi;
    __asm__ __volatile__("rdtsc"
            : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}


uint64_t get_time()
{
    struct timespec time1 = {0, 0};
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time1);
    uint64_t ns = time1.tv_sec * 1000000000 + time1.tv_nsec;

    return ns;
}