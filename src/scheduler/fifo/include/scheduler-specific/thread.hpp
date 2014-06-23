#ifndef __FIFO_THREAD_HPP__
#define __FIFO_THREAD_HPP__

#include <cstddef>
#include <cstdint>
#include <atomic>

namespace OS {

class Thread;

class SchedThread {
public:
  Thread * next = NULL;
  Thread * last = NULL;
  
  uint64_t nextTick = 0;
  bool isRunning = false;
};

}

#endif
