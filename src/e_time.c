#include "e_time.h"

#ifdef _WIN32
#include <windows.h>

static LARGE_INTEGER frequency;

void initialize_time_frequency() {
    QueryPerformanceFrequency(&frequency);
}

uint64_t etime_ms() {
    LARGE_INTEGER current;
    QueryPerformanceCounter(&current);
    return (float) current.QuadPart / (float) frequency.QuadPart;
}

#else // POSIX, Linux

#include <time.h>

uint64_t etime_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t) ts.tv_sec * 1000 + (uint64_t) ts.tv_nsec / 1000000;
}

#endif
