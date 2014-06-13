#ifndef __SCHEDULER_THREAD_HPP__
#define __SCHEDULER_THREAD_HPP__

#include <scheduler-specific/thread.hpp>
#include <arch-specific/thread.hpp>

namespace OS {

class Task;

class Thread : public ArchThread, public SchedThread {
public:
  static Thread * New(Task * owner, bool kernel); // @noncritical
  
  virtual void Delete(); // @noncritical
  Task * GetTask(); // @ambicritical
  
protected:
  Thread * taskNext, * taskLast;
  friend class Task;
  
  Thread(Task * owner, bool kernel); // @noncritical
  
private:
  Task * task;
};

}

#endif
