#ifndef __UNISTD_H__
#define __UNISTD_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void fork(void * function);
uint64_t getpid();
void usleep(uint64_t delay);

#ifdef __cplusplus
}
#endif

#endif
