#include <scheduler/user/user-task.hpp>

namespace OS {

UserTask * UserTask::New(UserCode * code) {
  // TODO: use SLAB here
  return new UserTask(code);
}

void UserTask::Delete() {
  // TODO: use SLAB here
  delete this;
}

bool UserTask::IsKernel() {
  return false;
}

CodeMap & UserTask::GetCodeMap() {
  return codeMap;
}

UserTask::UserTask(UserCode * code)
  : super(false), codeMap(this, code) {
}

}
