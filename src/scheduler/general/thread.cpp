#include <scheduler/general/thread.hpp>

namespace OS {

Thread * Thread::New(Task * owner, bool kernel) {
  return new Thread(owner, kernel); // TODO: use slab
}

Thread::Thread(Task * owner, bool kernel)
  : ArchThread(owner, kernel), task(owner) {
}

void Thread::Delete() {
  delete this; // TODO: use slab
}

Task * Thread::GetTask() {
  return task;
}

}
