#include <scheduler-specific/scheduler.hpp>
#include <scheduler/user/user-task.hpp>
#include <scheduler/user/kill-reasons.hpp>
#include <scheduler/general/thread.hpp>
#include <scheduler/general/hold-scope.hpp>
#include <critical>

namespace OS {

uint64_t SyscallLaunchThread(void * address, void * argument) {
  HoldScope scope;
  
  Thread * th = Thread::NewUser(scope.GetTask(), address, argument);
  scope.GetTask()->AddThread(th);
  uint64_t theId = th->GetThreadId();
  
  ScopeCritical critical;
  Scheduler::GetGlobal().AddThread(th);
  
  return theId;
}

void SyscallFork(void * address, void * argument) {
  HoldScope scope;
  
  UserTask * thisTask = static_cast<UserTask *>(scope.GetTask());
  Task * task = UserTask::New(thisTask->GetCodeMap().GetUserCode());
  Thread * th = Thread::NewUser(task, address, argument);
  
  task->AddThread(th);
  
  {
    ScopeCritical critical;
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
  
  Task::Exit(killReason);
}

void SyscallThreadExit() {
  ScopeCritical critical;
  Thread::Exit();
}

uint64_t SyscallGetPID() {
  HoldScope scope;
  return scope.GetTask()->GetPID();
}

uint64_t SyscallGetThreadID() {
  HoldScope scope;
  return scope.GetThread()->GetThreadId();
}

}
