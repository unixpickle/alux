#include <syscall/scheduler.hpp>
#include <scheduler-specific/scheduler.hpp>
#include <scheduler/user/user-task.hpp>
#include <scheduler/user/kill-reasons.hpp>
#include <scheduler/general/thread.hpp>
#include <scheduler/general/hold-scope.hpp>
#include <critical>

namespace OS {

ReturnValue SyscallLaunchThread(ArgList & args) {
  HoldScope scope;
  
  void * address = (void *)args.PopVirtAddr();
  void * argument = (void *)args.PopVirtAddr();
  
  Thread * th = Thread::NewUser(scope.GetTask(), address, argument);
  scope.GetTask()->AddThread(th);
  uint64_t theId = th->GetThreadId();
  
  Scheduler::GetGlobal().AddThread(th);
  th->Release();
  
  return ReturnValue::NewUInt64(theId);
}

void SyscallFork(ArgList & args) {
  HoldScope scope;
  
  void * address = (void *)args.PopVirtAddr();
  void * argument = (void *)args.PopVirtAddr();
  
  UserTask * thisTask = static_cast<UserTask *>(scope.GetTask());
  // TODO: this will change once i make tasks start off held
  Task * task = UserTask::New(thisTask->GetCodeMap().GetUserCode());
  Thread * th = Thread::NewUser(task, address, argument);
  
  task->AddThread(th);
  
  Scheduler::GetGlobal().AddThread(th);
  task->Unhold();
  th->Release();
  
  // NOTE: here, in the future, we will be opening a socket to the newly
  // launched task and we will return its file descriptor.
}

void SyscallExit(ArgList & args) {
  AssertCritical();
  
  bool wasError = args.PopBool();
  
  uint64_t killReason;
  if (wasError) killReason = KillReasons::UserError;
  else killReason = KillReasons::Natural;
  
  Task::Exit(killReason);
}

void SyscallThreadExit() {
  AssertCritical();
  Thread::Exit();
}

ReturnValue SyscallGetPID() {
  HoldScope scope;
  return ReturnValue::NewUInt64(scope.GetTask()->GetPID());
}

ReturnValue SyscallGetThreadID() {
  HoldScope scope;
  return ReturnValue::NewUInt64(scope.GetThread()->GetThreadId());
}

ReturnValue SyscallGetTaskCount() {
  AssertCritical();
  return ReturnValue::NewUInt64(Task::GetCounter());
}

ReturnValue SyscallGetThreadCount() {
  ScopeCritical critical;
  return ReturnValue::NewUInt64(Thread::GetCounter());
}

}
