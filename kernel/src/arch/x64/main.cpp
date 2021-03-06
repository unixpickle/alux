#include "main.hpp"
#include "executable.hpp"
#include "program-image.hpp"
#include "../../tasks/user-task.hpp"
#include "../../syscall/handler.hpp"
#include "../../memory/page-fault.hpp"
#include "../../scheduler/rr-scheduler.hpp"
#include <anarch/x64/multiboot-region-list>
#include <anarch/x64/init>
#include <anarch/api/domain-list>
#include <anarch/api/clock-module>
#include <anarch/api/clock>
#include <anarch/api/panic>
#include <anarch/critical>
#include <anarch/stream>
#include <ansa/macros>
#include <ansa/cstring>

extern "C" {

void AluxMainX64(void * mbootPtr) {
  anarch::x64::InitializeSingletons();
  
  anarch::SetGlobalPageDelegate(Alux::HandlePageFault);
  
  anarch::StreamModule::GetGlobal().Load();
  anarch::cout << "AluxMainX64(" << (uint64_t)mbootPtr << ")"
    << anarch::endl;
  
  // this stack is going to be preserved throughout the runtime of the OS, so
  // it is acceptable to store the boot info here
  
  Alux::x64::ProgramImage image;
  
  anarch::cout << "kernelEnd=" << image.GetKernelEnd() << " programStart="
    << image.GetProgramStart() << " programEnd=" << image.GetProgramEnd()
    << anarch::endl;
  
  anarch::x64::MultibootRegionList regions(mbootPtr);
  anarch::x64::BootInfo bootInfo(regions, image.GetProgramEnd());
  
  anarch::x64::SetBootInfo(&bootInfo);
  
  // load the modules we use
  anarch::DomainList::GetGlobal().Load();
  anarch::ClockModule::GetGlobal().Load();
  anarch::SyscallModule::GetGlobal().Load();
  
  // set up our syscall handler
  anarch::SyscallModule::GetGlobal().SetHandler(Alux::SyscallHandler);
  
  anarch::cout << "finished loading anarch modules!" << anarch::endl;
  
  Alux::RRScheduler scheduler;
  
  // create user task
  anarch::UserMap & map = anarch::UserMap::New();
  Alux::x64::Executable exec(image.GetProgramStart(), image.GetProgramSize());
  Alux::UserTask & task = Alux::UserTask::New(exec, map, 0, scheduler);
  if (!task.AddToScheduler()) {
    anarch::Panic("AluxMainX64() - failed to add task to scheduler");
  }
  
  // create user thread (which consumes a reference to our task)
  void * entry = task.GetExecutableMap().GetEntryPoint();
  anarch::State & state = anarch::State::NewUser((void (*)())entry);
  task.Retain();
  Alux::Thread & thread = Alux::Thread::New(task, state);
  if (!thread.AddToTask()) {
    anarch::Panic("AluxMainX64() - failed to add thread to task");
  }
  thread.AddToScheduler();
  
  // release user task data
  thread.Release();
  task.Unhold();
  
  // and boom, run our task!
  scheduler.Run();
}

}
