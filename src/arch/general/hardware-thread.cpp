#include <arch/general/hardware-thread.hpp>
#include <scheduler/general/task.hpp>
#include <critical>
#include <lock>

namespace OS {

HardwareThread::HardwareThread() : thread(NULL) {
}

void HardwareThread::SetThread(Thread * th) {
  AssertCritical();
  HardwareThread::GetCurrent().thread = th;
}

Thread * HardwareThread::GetThread() {
  AssertCritical();
  return HardwareThread::GetCurrent().thread;
}

}
