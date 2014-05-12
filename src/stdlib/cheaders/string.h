#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>

void * memset(void * ptr, int value, size_t number);
void bzero(void * ptr, size_t size);
void * memcpy(void * dst, const void * src, size_t n);

#endif

