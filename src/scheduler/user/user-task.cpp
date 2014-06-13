#include <scheduler/user/user-task.hpp>

namespace OS {

UserTask * UserTask::New() {
  // TODO: use SLAB here
  return new UserTask(false);
}

void UserTask::Delete() {
  // TODO: use SLAB here
  delete this;
}

bool UserTask::IsKernel() {
  return false;
}

}
