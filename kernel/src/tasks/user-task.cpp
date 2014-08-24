#include "user-task.hpp"
#include <anarch/critical>

namespace Alux {

UserTask * UserTask::New(Executable & e, anarch::UserMap & m, Scheduler & s) {
  AssertNoncritical();
  UserTask * res = new UserTask(e, m, s);
  if (!res) return NULL;
  if (!res->Init()) {
    delete res;
    return NULL;
  }
  return res;
}

anarch::UserMap & UserTask::GetMemoryMap() const {
  return memoryMap;
}

bool UserTask::IsUserTask() const {
  return true;
}

void UserTask::Dealloc() {
  AssertNoncritical();
  Deinit();
  delete this;
}

UserTask::UserTask(Executable & e, anarch::UserMap & m, Scheduler & s)
  : Task(s), memoryMap(m), executableMap(e.GenerateMap(m)) {
}

UserTask::~UserTask() {
  memoryMap.Delete();
  executableMap.Delete();
}

}
