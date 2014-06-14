#ifndef __X64_ACPI_MODULE_HPP__
#define __X64_ACPI_MODULE_HPP__

#include <module/module.hpp>

namespace OS {

namespace x64 {

class ACPIModule : public Module {
  static void InitGlobal();
  static ACPIModule & GetGlobal();
  
  virtual void Initialize();
  virtual Module ** GetDependencies(size_t & count);
};

}

}

#endif
