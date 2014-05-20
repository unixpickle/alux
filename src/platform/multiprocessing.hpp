#ifndef __PLATFORM_MULTIPROCESSING_HPP__
#define __PLATFORM_MULTIPROCESSING_HPP__

#include <scheduler/cpu.hpp>
#include <cstdint>

namespace OS {

typedef uint64_t ProcessorID;

class Processor {
public:
  CPUInfo info;
  
  /**
   * Returns the current processor structure.
   * @critical
   */
  static Processor * CurrentProcessor();
  
  /**
   * Returns a unique identifier for this CPU.
   * @ambicritical
   */
  virtual ProcessorID GetId() = 0;
  
  /**
   * Returns the "priority" of the CPU. The lower the priority, the more likely
   * the scheduler is to use this CPU for running tasks. This can be used to
   * eliminate the usage of hypercores unless it becomes necessary.
   * @ambicritical
   */
  virtual int GetPriority() = 0;
  
  /**
   * Sends an IPI to this CPU and receives a response. Make sure that you are
   * not holding any locks when you call this, since it will lock the CPU until
   * a response is received.
   * @ambicritical
   */
  virtual void * SendIPI(void * object) = 0;
  
};

/**
 * Returns the number of processors on the system.
 */
int ProcessorCount();

/**
 * Returns a reference to a processor at a given index.
 */
Processor & GetProcessor(int index);

}

#endif
