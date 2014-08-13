#include "incremental-id-pool.hpp"
#include <anarch/critical>

namespace OS {

TaskId IncrementalIdPool::Alloc(Task & task) {
  AssertNoncritical();
  anarch::ScopedLock scope(lock);
  tasks.Add(&task.pidPoolLink);
  return step++;
}

void IncrementalIdPool::Free(Task & task) {
  AssertNoncritical();
  anarch::ScopedLock scope(lock);
  tasks.Remove(&task.pidPoolLink);
}

Task * IncrementalIdPool::GetTask(TaskId ident) {
  AssertNoncritical();
  anarch::ScopedLock scope(lock);
  auto iter = tasks.GetStart();
  auto end = tasks.GetEnd();
  while (iter != end) {
    if ((*iter).GetId() == ident) {
      return &(*iter);
    }
    ++iter;
  }
  return NULL;
}

}
