#ifndef __PLATFORM_X64_PIT_X64_HPP__
#define __PLATFORM_X64_PIT_X64_HPP__

#include "../interrupts/lapic-x64.hpp"
#include "../interrupts/ioapic-x64.hpp"
#include "../interrupts/int-vectors-x64.hpp"
#include "../interrupts/irt-x64.hpp"
#include <platform/time.hpp>

namespace OS {

namespace x64 {

class PIT : public Clock {
private:
  uint64_t ticks;
  uint8_t divide;
  void IntHandler();
  
public:
  static void Initialize();
  static PIT & GetGlobal();
  static void Register();
  static void Deregister();
  
  PIT();
  
  void SetDivisor(uint16_t div); // 11932 for ~100Hz
  
  virtual uint64_t GetTime();
  virtual uint64_t GetTicksPerMin();
  
  friend void PitInterruptHandler();
};

void PitInterruptHandler();

}

}

#endif
