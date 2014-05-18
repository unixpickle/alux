#ifndef __PLATFORM_X64_INT_HANDLERS_X64_H__
#define __PLATFORM_X64_INT_HANDLERS_X64_H__

#include "idt-x64.hpp"
#include "raw-handlers-x64.hpp"
#include "../common-x64.hpp"
#include <platform/failure.hpp>
#include <iostream>

extern "C" {

void InterruptCoded(void * caller, uint64_t vector, uint64_t code);
void InterruptRegular(void * caller, uint64_t vector);
void InterruptDummy(void * caller, uint64_t vector);

void _InterruptCoded(void * caller, uint64_t vector, uint64_t code);
void _InterruptRegular(void * caller, uint64_t vector);
void _InterruptDummy(void * caller, uint64_t vector);

}

namespace OS {

namespace x64 {

typedef void (* IntRoutine)();

void InitializeIDT();
InterruptTable & GetGlobalIDT();
void ConfigureDummyIDT();
void ConfigureRealIDT();

void SetIntRoutine(uint8_t vec, IntRoutine handler);
IntRoutine GetIntRoutine(uint8_t vec);
void UnsetIntRoutine(uint8_t vec);

}

}

#endif
