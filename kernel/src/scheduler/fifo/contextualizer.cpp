#include <scheduler/fifo/contextualizer.hpp>
#include <scheduler/general/thread.hpp>
#include <scheduler/general/task.hpp>
#include <arch/general/hardware-thread.hpp>
#include <arch/general/address-space.hpp>
#include <arch/general/global-map.hpp>
#include <critical>

namespace OS {

Contextualizer::Contextualizer() {
  currentThread = HardwareThread::GetThread();
}

bool Contextualizer::SetNewThread(Thread * nt) {
  AssertCritical();
  
  if (!nt->isRunning || nt == currentThread) {
    if (!nt->Retain()) {
      return false;
    }
    newThread = nt;
    return true;
  }
  return false;
}

Thread * Contextualizer::GetNewThread() {
  return newThread;
}

void Contextualizer::SwitchRunningInfo() {
  AssertCritical();
  if (currentThread != newThread) {
    if (currentThread) {
      currentThread->isRunning = false;
    }
    if (newThread) {
      newThread->isRunning = true;
    }
  }
  HardwareThread::SetThread(newThread);
}

void Contextualizer::SwitchAddressSpace() {
  AssertCritical();
  
  Task * curTask = currentThread ? currentThread->GetTask() : NULL;
  Task * newTask = newThread ? newThread->GetTask() : NULL;
  
  if (curTask == newTask) return;
  
  bool wasGlobal = curTask ? curTask->IsKernel() : true;
  bool isGlobal = newTask ? newTask->IsKernel() : true;
  
  if (isGlobal && wasGlobal) return;
  
  if (newTask) {
    newTask->GetAddressSpace().Set();
  } else {
    GlobalMap::GetGlobal().Set();
  }
}

void Contextualizer::ReleaseLast() {
  AssertCritical();
  
  if (currentThread) {
    currentThread->Release();
  }
}

}
