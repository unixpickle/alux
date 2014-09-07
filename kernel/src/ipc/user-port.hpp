#ifndef __ALUX_USER_PORT_HPP__
#define __ALUX_USER_PORT_HPP__

#include "port.hpp"
#include <anidmap/id-object>
#include <anidmap/maps>
#include <ansa/linked-list>

namespace Alux {

class Thread;

class UserPort : public Port, public anidmap::IdObject {
public:
  /**
   * Read the next pending data packet from the most recent pending port of the
   * current thread.
   * @noncritical
   */
  static UserPort * ReadNext(Data &);
  
  static UserPort * New(Thread &); // @noncritical
  virtual void Delete(); // @noncritical
  virtual void Delete(bool remove); // @noncritical
  
protected:
  template <class T, int C>
  friend class anidmap::HashMap;
  ansa::LinkedList<UserPort>::Link hashMapLink;
  
  virtual void Send(const Data & data); // @noncritical
  
private:
  UserPort(Thread &);
  
  bool Read(Data & out); // @noncritical
  
  bool Init();
  void Deinit();
  
  Thread & thread;
  ansa::LinkedList<UserPort>::Link waitingLink;
  
  // synchronized by thread's waitingLock
  bool signaled = false;
  Data data;
};

}

#endif
