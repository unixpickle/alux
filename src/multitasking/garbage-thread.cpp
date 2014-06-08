#include <multitasking/garbage-thread.hpp>
#include <utilities/critical.hpp>

namespace OS {

GarbageThread::GarbageThread() : itemsLock(0), firstTask(NULL) {
}

Thread * GarbageThread::GetThread() {
  return thread;
}

void PushObject(Destructable * obj) {
  // TODO: this guy!
}

Destructable * PopObject() {
  // TODO: this guy
}

void SetThread(Thread * th) {
  thread = th;
}

void MainMethod() {
  while (1) {
    ScopeCritical critical;
    ScopeCriticalLock lock(&objectsLock);
    // TODO: stuff here
  }
}

}
