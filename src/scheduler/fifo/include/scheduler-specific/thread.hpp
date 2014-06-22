#ifndef __FIFO_THREAD_HPP__
#define __FIFO_THREAD_HPP__

#include <cstddef>
#include <cstdint>
#include <atomic>

namespace OS {

class Thread;

class SchedThread {
public:
  SchedThread() : nextTick(0) {}
  
  Thread * next = NULL;
  Thread * last = NULL;
  
  Atomic<uint64_t> nextTick;
  bool isRunning = false;
};

}

#endif
