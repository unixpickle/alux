#ifndef __SCHEDULER_DESCRIPTOR_HPP__
#define __SCHEDULER_DESCRIPTOR_HPP__

#include <cstdint>

namespace OS {

class Task;
class Message;

class Descriptor {
public:
  /**
   * Create a new descriptor given a task and an allocated descriptor number.
   * @param task A referenced task.
   * @param descriptor The descriptor.
   * @noncritical
   */
  Descriptor(Task * task, uint64_t descriptor);
  
  /**
   * Release the resources used by a descriptor.
   * @noncritical
   */
  ~Descriptor();
  
  /**
   * Returns a referenced version of the task or NULL if the owning task has
   * been killed.
   * @critical
   */
  virtual Task * GetTask();
  
  /**
   * Returns the descriptor's numerical representation.
   * @ambicritical
   */
  virtual uint64_t GetDescriptor();
  
  /**
   * Connect this descriptor to a remote descriptor.
   * @param remote A retained descriptor to connect to.
   * @return true if the connection was made, or false if the remote was
   * already connected.
   * @critical
   */
  virtual bool Connect(Descriptor * remote);
  
  /**
   * Retain the descriptor. Returns false if the descriptor was being closed
   * when you called this.
   * @critical
   */
  virtual bool Retain();
  
  /**
   * Release the descriptor.
   * @noncritical
   */
  virtual void Release();
  
  /**
   * Request that the descriptor be closed.
   * @noncritical
   */
  virtual void Close();

protected:
  Task * task;
  uint64_t descriptor;
  
  uint64_t remoteLock;
  bool hasConnected;
  Descriptor * remote;
  
  uint64_t messagesLock;
  Message * firstMessage;
  
  uint64_t closeLock;
  uint64_t isClosed;
  uint64_t refCount;
  uint64_t closeCode;
};

}

#endif
