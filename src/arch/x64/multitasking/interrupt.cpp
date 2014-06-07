#include <arch/x64/smp/cpu-list.hpp>
#include <scheduler/scheduler.hpp>
#include <utilities/critical.hpp>

namespace OS {

namespace x64 {

void MultitaskingInterrupt() {
  AssertCritical();
  // TODO: here, switch to the CPU stack, unset the current job, and invoke the
  // scheduler.
}

}

}