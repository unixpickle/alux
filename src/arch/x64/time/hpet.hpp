#ifndef __X64_HPET_HPP__
#define __X64_HPET_HPP__

#include <arch/general/clock.hpp>
#include <memory/easy-map.hpp>

namespace OS {

namespace x64 {

class HPET : public Clock {
private:
  EasyMap * map;
  uint64_t clockSpeed;

public:
  HPET();
  ~HPET();
  
  static bool IsSupported(); // @noncritical
  static void Initialize(); // @noncritical
  static HPET & GetGlobal(); // @noncritical
  
  virtual uint64_t GetTime();
  virtual uint64_t GetTicksPerMin();
  
  void Start();
  void Stop();
};

}

}

#endif
