#include <arch/general/cpu.hpp>
#include <scheduler/scheduler.hpp>
#include <utilities/critical.hpp>

namespace OS {

void SetupTask(Task * t) {
  Scheduler::Scheduler & sched = Scheduler::Scheduler::GetGlobal();
  t->userInfo = sched.InfoForJobGroup(t);
}

void LaunchThread(Thread * th) {
  Scheduler::Scheduler & sched = Scheduler::Scheduler::GetGlobal();
  th->userInfo = sched.InfoForJob(th);
  sched.AddJob(th);
}

void ResignThread() {
  AssertCritical();
  CPU & cpu = CPU::GetCurrent();
  cpu.SetThread(NULL);
  cpu.ClearTimeout();
  Scheduler::Scheduler & sched = Scheduler::Scheduler::GetGlobal();
  sched.Tick();
}

}