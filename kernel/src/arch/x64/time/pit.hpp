#ifndef __X64_PIT_HPP__
#define __X64_PIT_HPP__

#include <arch/general/clock.hpp>
#include <atomic>

namespace OS {

namespace x64 {

class PIT : public Clock {
protected:
  Atomic<uint64_t> counter;
  uint16_t divide = 0;
  void IntHandler();

public:
  static void Initialize();
  static PIT & GetGlobal();
  
  static void Start();
  static void Stop();
  
  PIT();
  
  void SetDivisor(uint16_t div);
  
  virtual uint64_t GetTime();
  virtual uint64_t GetTicksPerMin();
  
  friend void PitInterruptHandler();
};

void PitInterruptHandler();

}

}

#endif
