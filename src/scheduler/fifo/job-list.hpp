#ifndef __FIFO_JOB_LIST_HPP__
#define __FIFO_JOB_LIST_HPP__

#include <scheduler/job.hpp>
#include <cstddef>

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

}

}

}

#endif
