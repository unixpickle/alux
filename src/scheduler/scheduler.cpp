#include <scheduler/scheduler.hpp>
#include <scheduler/fifo/scheduler.hpp>
#include <new>

namespace OS {

namespace Scheduler {

static FIFO::Scheduler scheduler;

void Scheduler::Initialize() {
  new(&scheduler) FIFO::Scheduler();
}

Scheduler & Scheduler::GetGlobal() {
  return scheduler;
}

}

}