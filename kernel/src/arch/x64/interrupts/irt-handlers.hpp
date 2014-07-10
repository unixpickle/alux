#ifndef __X64_IRT_HANDLERS_HPP__
#define __X64_IRT_HANDLERS_HPP__

#include <cstdint>

extern "C" {

void InterruptCoded(void * caller, uint64_t vector, uint64_t code,
                    uint64_t cs);
void InterruptRegular(void * caller, uint64_t vector, uint64_t cs);
void InterruptDummy(void * caller, uint64_t vector, uint64_t cs);

void _InterruptCoded(void * caller, uint64_t vector, uint64_t code,
                     uint64_t cs);
void _InterruptRegular(void * caller, uint64_t vector, uint64_t cs);
void _InterruptDummy(void * caller, uint64_t vector, uint64_t cs);

}

#endif
