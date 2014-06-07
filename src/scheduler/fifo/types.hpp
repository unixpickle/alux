#ifndef __FIFO_TYPE_HPP__
#define __FIFO_TYPE_HPP__

#include <scheduler/types.hpp>

namespace OS {

namespace Scheduler {

namespace FIFO {

class JobList {
public:
  JobList();
  
  void PushJob(Job * job); // @critical
  Job * NextJob(); // @critical
  bool RemoveJob(Job * job); // @critical
  
  class Node {
  public:
    Node * next;
    Node * last;
    Job * job;
    
    Node(Job * aJob) : next(NULL), last(NULL), job(aJob) {}
  };
  
protected:
  Node * firstNode; // or NULL
  Node * lastNode; // or NULL
};

class JobInfo : public UserInfo {
public:
  static JobInfo * ForJob(Job * j);
  
  JobInfo(Job * job);
  JobList::Node node;
  
  uint64_t timerDeadline;
};

class Scheduler : public OS::Scheduler::Scheduler {
public:
  static const int Jiffy = 6000; // 6000 times a minute (100 Hz)
  
  Scheduler();

  virtual UserInfo * InfoForJob(Job * aJob);
  virtual UserInfo * InfoForJobGroup(JobGroup * aJobGroup);
  virtual void SetTimer(uint64_t fireTime, bool prec);
  virtual bool UnsetTimer(Job * job);
  virtual void Tick();
  virtual void AddJob(Job * job);
  virtual void DeleteJob(Job * job);

protected:
  uint64_t lock OS_ALIGNED(8); // @critical
  JobList jobList;
};

}

}

}

#endif
