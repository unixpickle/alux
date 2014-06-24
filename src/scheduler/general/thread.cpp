#include <scheduler/general/thread.hpp>
#include <arch/general/state.hpp>

namespace OS {

Thread * Thread::NewUser(Task * owner, void * call) {
  return new Thread(owner, false, call);
}

Thread * Thread::NewUser(Task * owner, void * call, void * arg) {
  return new Thread(owner, false, call, arg);
}

Thread * Thread::NewKernel(Task * owner, void * call) {
  return new Thread(owner, true, call);
}

Thread * Thread::NewKernel(Task * owner, void * call, void * arg) {
  return new Thread(owner, true, call, arg);
}

Thread::~Thread() {
  state->Delete();
}

Thread::Thread(Task * owner, bool kernel, void * func) : task(owner) {
  if (kernel) {
    state = State::NewKernel(func);
  } else {
    state = State::NewUser(func);
  }
}

Thread::Thread(Task * owner, bool kernel, void * func, void * arg)
  : task(owner) {
  if (kernel) {
    state = State::NewKernel(func, arg);
  } else {
    state = State::NewUser(func, arg);
  }
}

void Thread::Delete() {
  delete this;
}

Task * Thread::GetTask() {
  return task;
}

State & Thread::GetState() {
  return *state;
}

uint64_t Thread::GetThreadId() {
  return threadId;
}

}
