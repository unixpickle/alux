#ifndef __UNISTD_H__
#define __UNISTD_H__

#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

void fork(void * function);
uint64_t getpid();

#ifdef __cplusplus
}
#endif

#endif
