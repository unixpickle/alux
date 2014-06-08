#ifndef __MULTITASKING_GARBAGE_THREAD_HPP__
#define __MULTITASKING_GARBAGE_THREAD_HPP__

#include <multitasking/thread.hpp>
#include <multitasking/destructable.hpp>

namespace OS {

class KernelTask;

class GarbageThread {
public:
  GarbageThread();
  
  Thread * GetThread(); // @ambicritical
  void PushObject(Destructable * obj); // @critical
  
protected:
  Thread * thread;
  
  uint64_t objectsLock OS_ALIGNED(8);
  Destructable * first;
  
  Destructable * PopObject(); // @critical, doesn't lock
  
  void SetThread(Thread * th); // @ambicritical
  void MainMethod(); // @noncritical
  
  friend class KernelTask;
};

}

#endif