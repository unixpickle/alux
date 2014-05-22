#ifndef __PLATFORM_X64_TSC_X64_HPP__
#define __PLATFORM_X64_TSC_X64_HPP__

#include "../multiprocessing/cpu-list-x64.hpp"
#include "../common-x64.hpp"
#include "pit-x64.hpp"

namespace OS {
namespace x64 {

class TSC : public Clock {
private:
  uint64_t ticksPerMin;
  
public:
  static bool IsSupported(); // checks for invariant TSC
  static void Initialize();
  static TSC & GetGlobal();
  
  TSC();
  
  virtual uint64_t GetTime();
  virtual uint64_t GetTicksPerMin();
};

}
}

#endif
