#ifndef E_TIME_H
#define E_TIME_H

#include <stdint.h>

uint64_t etime_ms();

#ifdef _WIN32
void initialize_time_frequency();
#endif // _WIN32

#endif // E_TIME_H
