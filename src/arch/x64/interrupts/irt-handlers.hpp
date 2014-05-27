#ifndef __X64_IRT_HANDLERS_HPP__
#define __X64_IRT_HANDLERS_HPP__

#include <cstdint>

extern "C" {

void InterruptCoded(void * caller, uint64_t vector, uint64_t code);
void InterruptRegular(void * caller, uint64_t vector);
void InterruptDummy(void * caller, uint64_t vector);

void _InterruptCoded(void * caller, uint64_t vector, uint64_t code);
void _InterruptRegular(void * caller, uint64_t vector);
void _InterruptDummy(void * caller, uint64_t vector);

}

#endif
