#ifndef __FIFO_JOB_LIST_HPP__
#define __FIFO_JOB_LIST_HPP__

#include <common>
#include <cstddef>

namespace OS {

class Job;

namespace FIFO {

class JobList {
public:
  JobList();
  ~JobList();
  
  void PushJob(Job * job); // @critical
  Job * NextJob(); // @critical
  bool RemoveJob(Job * job); // @critical
  
  class Node {
    Node * next;
    Node * last;
    Job * job;
    JobList * jobList; // or NULL
    
    Node() : next(NULL), last(NULL), job(NULL), jobList(NULL) {}
  };
  
protected:
  uint64_t lock OS_ALIGNED(8); // @critical
  
  Node * firstNode; // or NULL
};

}

}

#endif