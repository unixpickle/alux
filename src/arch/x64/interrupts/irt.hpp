#ifndef __X64_IRT_HPP__
#define __X64_IRT_HPP__

#include <arch/x64/interrupts/types.hpp>

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
  
  Routine & operator[](uint8_t idx);
  void Unset(uint8_t idx);

protected:
  Routine * routines;
  
  void ConfigureDummy();
  void Configure();
};

}

}

#endif
