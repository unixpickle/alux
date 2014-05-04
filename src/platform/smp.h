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

namespace OS {

typedef uintptr_t ProcessorID;

class Processor {
private:
  void * ui;
  ProcessorID identifier;
  int priority;
  bool isActive;
  
public:
  
  /**
   * Initialize the processor list. This method should be synchronous.
   */
  static void InitializeProcessors();
  
  /**
   * Return the number of processors on the system.
   */
  static int ProcessorCount();
  
  /**
   * Returns an array of processors that are on the system.
   */
  static Processor * ProcessorList();
  
  /**
   * Returns the current processor structure.
   */
  static Processor * CurrentProcessor();
  
  /**
   * Construct a new processor.
   */
  Processor(void * ui, ProcessorID identifier, int priority);
  
  /**
   * Returns a unique identifier for this CPU.
   */
  ProcessorID GetID() const;
  
  /**
   * Returns the "priority" of the CPU. The lower the priority, the more likely
   * the scheduler is to use this CPU for running tasks. This can be used to
   * eliminate the usage of hypercores unless it becomes necessary.
   */
  int GetPriority() const;
  
  /**
   * Returns the user info pointer.
   */
  void * GetUserInfo() const;
  
  /**
   * Returns whether or not the CPU is active. This ought to be atomic.
   */
  bool GetIsActive() const;
  
  /**
   * Set whether or not the CPU is active. This ought to be atomic.
   */
  void SetIsActive(bool flag);
  
  /**
   * Hangs this CPU until it gets an interrupt.
   */
  void Halt();
  
  /**
   * Sends an IPI to this CPU indicating that a piece of memory has been
   * modified. The OS::HandleMemoryIPI method should be called.
   */
  void SendMemoryIPI();
};

}
