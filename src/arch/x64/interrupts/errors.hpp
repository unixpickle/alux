#ifndef __X64_INTERRUPT_ERRORS_HPP__
#define __X64_INTERRUPT_ERRORS_HPP__

#include <module/module.hpp>

namespace OS {

namespace x64 {

class InterruptErrors : public Module {
public:
  static void InitGlobal();
  static InterruptErrors & GetGlobal();
  
  virtual void Initialize();
  virtual DepList GetDependencies();
  
private:
  static void HandleNMI();
  static void HandleMCE();
};

}

}

#endif
