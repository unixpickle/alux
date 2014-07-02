#ifndef __FIFO_CONTEXTUALIZER_HPP__
#define __FIFO_CONTEXTUALIZER_HPP__

#include <cstddef>

namespace OS {

class Thread;

class Contextualizer {
public:
  Contextualizer();
  
  bool SetNewThread(Thread * nt);
  Thread * GetNewThread();
  
  void SwitchRunningInfo();
  void SwitchAddressSpace();
  void ReleaseLast();

private:
  Thread * currentThread;
  Thread * newThread = NULL;
};

}

#endif
