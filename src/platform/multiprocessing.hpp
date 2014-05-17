/*
 * Copyright (c) 2014, Alex Nichol and Alux contributors.
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

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
