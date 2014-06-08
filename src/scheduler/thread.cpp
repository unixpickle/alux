#include <scheduler/thread.hpp>

namespace OS {

Thread::Thread(Task * owner) : task(owner) {
}

Thread::~Thread() {
}

Task * Thread::GetTask() {
  return task;
}

}
