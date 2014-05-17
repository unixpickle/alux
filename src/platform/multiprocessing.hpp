#include <scheduler/cpu.hpp>
#include <cstdint>

namespace OS {

typedef uint64_t ProcessorID;

class Processor {
public:
  CPUInfo info;
  
  /**
   * Returns the current processor structure.
   */
  static Processor * CurrentProcessor();
  
  /**
   * Returns a unique identifier for this CPU.
   */
  virtual ProcessorID GetID() = 0;
  
  /**
   * Returns the "priority" of the CPU. The lower the priority, the more likely
   * the scheduler is to use this CPU for running tasks. This can be used to
   * eliminate the usage of hypercores unless it becomes necessary.
   */
  virtual int GetPriority() = 0;
  
  /**
   * Sends an IPI to this CPU and receives a response. Make sure that you are
   * not holding any locks when you call this, since it will lock the CPU until
   * a response is received.
   */
  virtual void * SendSyncIPI(void * object);
  
  /**
   * Sends an IPI to this CPU and does not wait for a response.
   */
  virtual void SendAsyncIPI(void * object);
  
};

/**
 * Initialize the entire multiprocessing system. This will be called by the
 * kernel entry point after the system memory allocator has been configured.
 */
void InitializeProcessors();

/**
 * Returns the number of processors on the system.
 */
int ProcessorCount();

/**
 * Returns a reference to a processor at a given index.
 */
Processor & GetProcessor(int index);

}
