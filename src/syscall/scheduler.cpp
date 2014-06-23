#include <scheduler-specific/scheduler.hpp>
#include <scheduler/user/user-task.hpp>
#include <scheduler/user/kill-reasons.hpp>
#include <scheduler/general/thread.hpp>
#include <scheduler/general/hold-scope.hpp>
#include <critical>

namespace OS {

void SyscallLaunchThread(void * address, void * argument) {
  HoldScope scope;
  if (!scope.DidHold()) return;
  
  Thread * th = Thread::NewUser(scope.GetTask(), address, argument);
  
  ScopeCritical critical;
  scope.GetTask()->AddThread(th);
  Scheduler::GetGlobal().AddThread(th);
}

void SyscallFork(void * address, void * argument) {
  HoldScope scope;
  if (!scope.DidHold()) return;
  
  UserTask * thisTask = static_cast<UserTask *>(scope.GetTask());
  Task * task = UserTask::New(thisTask->GetCodeMap().GetUserCode());
  Thread * th = Thread::NewUser(task, address, argument);
  
  {
    ScopeCritical critical;
    task->AddThread(th);
    Scheduler::GetGlobal().AddThread(th);
    task->Release();
  }
  
  // NOTE: here, in the future, we will be opening a socket to the newly
  // launched task and we will return its file descriptor.
}

void SyscallExit(bool wasError) {
  ScopeCritical critical;
  
  uint64_t killReason;
  if (wasError) killReason = KillReasons::UserError;
  else killReason = KillReasons::Natural;
  
  Scheduler::GetGlobal().ExitTask(killReason);
}

void SyscallThreadExit() {
  ScopeCritical critical;
  Scheduler::GetGlobal().ExitThread();
}

}
