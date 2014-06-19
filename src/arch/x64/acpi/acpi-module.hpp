#ifndef __X64_ACPI_MODULE_HPP__
#define __X64_ACPI_MODULE_HPP__

#include <module/module.hpp>

namespace OS {

namespace x64 {

class ACPIModule : public Module {
public:
  static void InitGlobal();
  static ACPIModule & GetGlobal();
  
protected:
  virtual void Initialize();
  virtual DepList GetDependencies();
};

}

}

#endif
