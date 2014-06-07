#include <cstdint>
#include <arch/x64/interrupts/lapic.hpp>
#include <arch/x64/interrupts/irt.hpp>
#include <arch/x64/interrupts/vectors.hpp>
#include <arch/x64/smp/cpu-list.hpp>

namespace OS {

static bool useIPI = false;
static bool hasPanicked = false;

static void PanicIPI();

void InitializePanic() {
  x64::IRT::GetGlobal()[x64::IntVectors::Panic] = PanicIPI;
  useIPI = true;
}

void Panic(const char * message) {
  __asm__("cli");
  if (useIPI) {
    if (__sync_fetch_and_or(&hasPanicked, 1)) {
      return;
    }
    
    // loop through CPUs
    x64::CPUList & list = x64::CPUList::GetGlobal();
    x64::LAPIC & lapic = x64::LAPIC::GetCurrent();
    for (int i = 0; i < list.GetCount(); i++) {
      x64::CPU & cpu = list[i];
      if (cpu.GetAPICID() == lapic.GetId()) continue;
      lapic.SendIPI(cpu.GetAPICID(), x64::IntVectors::Panic);
    }
  }
    
  // write the error to the screen
  uint16_t * buf = (uint16_t *)0xb8000;
  int i = 0;
  const char * prefix = "PANIC: ";
  while (*prefix) {
    buf[i++] = *prefix | 0xf200;
    prefix++;
  }
  while (*message) {
    buf[i++] = *message | 0xf900;
    message++;
  }
  buf[i] = 0xf920;
  
  __asm__("hlt");
}

static void PanicIPI() {
  __asm__("cli\nhlt");
}


}
