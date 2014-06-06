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
    JobList * jobList;
    
    Node(Job * aJob) : next(NULL), last(NULL), job(aJob), jobList(NULL) {}
  };
  
protected:
  Node * firstNode; // or NULL
  Node * lastNode; // or NULL
};

class JobInfo : public UserInfo {
public:
  JobInfo(Job * job);
  JobList::Node node;
  Context * affinity;
};

class ContextInfo : public UserInfo {
public:
  bool isActive;
};

class Scheduler : public OS::Scheduler::Scheduler {
public:
  Scheduler(Context ** contexts, size_t count);

  virtual UserInfo * InfoForJob(Job * aJob);
  virtual UserInfo * InfoForJobGroup(JobGroup * aJobGroup);
  virtual void SetTimer(uint64_t fireTime, Job * job, bool prec);
  virtual void CollectGarbage();
  virtual void PushJob(Job * job);
  virtual Job * PopJob();
  virtual void DeleteJob(Job * job);

protected:
  Context ** contexts;
  size_t count;

  uint64_t lock OS_ALIGNED(8); // @critical
  JobList * jobLists;

  Context * LeastUtilizedContext(); // @ambicritical, locked already
  static bool & IsContextActive(Context * context);
};

}

}

}

#endif
