#ifndef __X64_PANIC_MODULE_HPP__
#define __X64_PANIC_MODULE_HPP__

#include <module/module.hpp>

namespace OS {

namespace x64 {

class PanicModule : public Module {
public:
  static void InitGlobal();
  static PanicModule & GetGlobal();
  
  void DistributeError();
  
protected:
  virtual void Initialize();
  virtual DepList GetDependencies();
  
  bool hasBeenInitialized;
  bool hasPanicked;
};

}

}

#endif
