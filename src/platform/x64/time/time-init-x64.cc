#include "time-init-x64.hpp"

namespace OS {
namespace x64 {

static int32_t calibrateRemaining OS_ALIGNED(8);

void InitializeTime() {
  CalibrateLapicTimers();

  Panic("TODO: here, setup a better time keeping mechanism.");
  if (TSC::IsSupported()) {
    TSC::Initialize();
  } else if (HPET::IsSupported()) {
    HPET::Initialize();
  } else {
    // resort to using the PIT
  }
}

void CalibrateLapicTimers() {
  cout << "OS::x64::CalibrateLapicTimers()" << endl;
  IRT::GetGlobal()[IntVectors::Calibrate] = CpuCalibrateLapic;

  LAPIC & lapic = LAPIC::GetCurrent();
  calibrateRemaining = CPUList::GetGlobal().GetCount();
  for (int i = 0; i < CPUList::GetGlobal().GetCount(); i++) {
    CPU & cpu = CPUList::GetGlobal()[i];
    if (cpu.apicId == lapic.GetId()) continue;
    lapic.SendIPI(cpu.apicId, IntVectors::Calibrate, 0, 1, 0);
  }
  CpuCalibrateLapic();
  
  while (calibrateRemaining);
  
  IRT::GetGlobal().Unset(IntVectors::Calibrate);
}

void CpuCalibrateLapic() {
  LAPIC & lapic = LAPIC::GetCurrent();
  PIT::GetGlobal().Sleep(1);
  lapic.WriteRegister(LAPIC::RegLVT_TMR, 0xff);
  lapic.WriteRegister(LAPIC::RegTMRINITCNT, 0xffffffff);
  PIT::GetGlobal().Sleep(50);

  uint64_t value = lapic.ReadRegister(LAPIC::RegTMRCURRCNT);
  lapic.WriteRegister(LAPIC::RegLVT_TMR, 0x10000);

  CPU & cpu = CPUList::GetGlobal().GetCurrent();
  cpu.timeInfo.lapicClock = (uint64_t)(0xffffffff - value) * 2;
  
  __asm__("lock decl (%0)" : : "r" (&calibrateRemaining));
}

}
}
