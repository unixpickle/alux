#ifndef __X64_IRT_HPP__
#define __X64_IRT_HPP__

#include <arch/x64/interrupts/types.hpp>
#include <module/module.hpp>

namespace OS {

namespace x64 {

/**
* Interrupt Routine Table -- my own invention
*/
class IRT : public Module {
public:
  typedef void (* Routine)();
  
  static void InitGlobal(); // @noncritical
  static IRT & GetGlobal(); // @ambicritical
  
  Routine & operator[](uint8_t idx); // @ambicritical, unsyrchronized
  void Unset(uint8_t idx); // @ambicritical, unsyrchronized

protected:
  Routine * routines;
  
  virtual void Initialize();
  virtual DepList GetDependencies();
  
  void ConfigureDummy();
  void Configure();
};

}

}

#endif
