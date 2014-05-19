#include "smp-init-x64.hpp"

namespace OS {

void InitializeProcessors() {
  x64::InitializeInterrupts();
  x64::ConfigureIOAPIC();
  x64::InitializeLAPIC();
  
  x64::PitSetDivisor(11932);
  x64::SetIntRoutine(x64::IntVectors::PIT, x64::PitInterruptHandler);
  x64::GetBaseIOAPIC().MapIRQ(0, x64::IntVectors::PIT);
  SetInterruptsEnabled(true);
  
  x64::SetupCpuList();
  x64::StartCpus();
  x64::CalibrateCpus();
  
  Panic("InitializeProcessors() - am I done, yet?");
}

}
