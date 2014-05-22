#ifndef __PLATFORM_X64_INT_HANDLERS_X64_H__
#define __PLATFORM_X64_INT_HANDLERS_X64_H__

#include "idt-x64.hpp"
#include "raw-handlers-x64.hpp"
#include "../common-x64.hpp"
#include <platform/failure.hpp>
#include <iostream>

namespace OS {

namespace x64 {

/**
 * Interrupt Routine Table -- my own invention
 */
class IRT {
public:
  typedef void (* Routine)();
  
  static void Initialize();
  static IRT & GetGlobal();
  
  IRT();
  ~IRT() {
    Panic("this is a singleton class");
  }
  
  void ConfigureDummy();
  void Configure();
  
  Routine & operator[](uint8_t idx);
  void Unset(int idx);
  
private:
  Routine * routines;
};

}

}

extern "C" {

void InterruptCoded(void * caller, uint64_t vector, uint64_t code);
void InterruptRegular(void * caller, uint64_t vector);
void InterruptDummy(void * caller, uint64_t vector);

void _InterruptCoded(void * caller, uint64_t vector, uint64_t code);
void _InterruptRegular(void * caller, uint64_t vector);
void _InterruptDummy(void * caller, uint64_t vector);

}

#endif
