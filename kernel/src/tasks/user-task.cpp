#include "user-task.hpp"
#include <anarch/critical>

namespace Alux {

UserTask & UserTask::New(Executable & e, anarch::UserMap & m, Identifier i, 
                         Scheduler & s) {
  AssertNoncritical();
  UserTask * res = new UserTask(e, m, i, s);
  assert(res != NULL);
  return *res;
}

anarch::UserMap & UserTask::GetMemoryMap() {
  return memoryMap;
}

bool UserTask::IsUserTask() {
  return true;
}

void UserTask::Dealloc() {
  Task::Dealloc();
  delete this;
}

UserTask::UserTask(Executable & e, anarch::UserMap & m, Identifier i, 
                   Scheduler & s)
  : Task(i, s), memoryMap(m), executableMap(e.GenerateMap(m)) {
}

UserTask::~UserTask() {
  memoryMap.Delete();
  executableMap.Delete();
}

}
