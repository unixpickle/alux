#include "platform-init-x64.hpp"

namespace OS {

void InitializePlatform() {
  x64::InitializeInterrupts();
  x64::ConfigureIOAPIC();
  x64::InitializeLAPIC();
  
  x64::PIT::Initialize();
  x64::PIT::GetGlobal().SetDivisor(11932);
  x64::PIT::GetGlobal().Register();
  SetInterruptsEnabled(true);
  
  x64::InitializeSMP();
  x64::InitializeFailure();
  x64::InitializeTime();
}

}
