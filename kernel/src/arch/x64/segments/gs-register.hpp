#ifndef __X64_GS_REGISTER_HPP__
#define __X64_GS_REGISTER_HPP__

#include <cstdint>

namespace OS {

namespace x64 {

class GSRegister {
public:
  static void SetBase(uint64_t index);
  static uint64_t GetBase();
  
  static bool ShouldSwap(uint64_t cs, uint64_t vector);
  static void Swap();
};

class ScopeSwapGS {
public:
  ScopeSwapGS(uint64_t cs, uint64_t vector);
  ~ScopeSwapGS();
  
private:
  bool shouldSwap;
};

}

}

#endif
