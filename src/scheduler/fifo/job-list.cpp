#include <scheduler/fifo/types.hpp>

namespace OS {

namespace Scheduler {

namespace FIFO {

JobList::JobList() : firstNode(NULL), lastNode(NULL) {
}

void JobList::PushJob(Job * job) {
  Node * node = &JobInfo::ForJob(job)->node;
  node->last = NULL;
  node->next = firstNode;
  if (firstNode) {
    firstNode->last = node;
  }
  firstNode = node;
}

Job * JobList::NextJob() {
  if (!lastNode) return NULL;
  Node * result = lastNode;
  lastNode = result->last;
  if (lastNode) {
    lastNode->next = NULL;
  } else {
    firstNode = NULL;
  }
  return result->job;
}

bool JobList::RemoveJob(Job * job) {
  Node * node = &JobInfo::ForJob(job)->node;  
  if (!node->next && !node->last) {
    if (node != firstNode) return false;
  }
  
  if (node->last) {
    node->last->next = node->next;
  } else {
    firstNode = node->next;
  }
  if (node->next) {
    node->next->last = node->last;
  } else {
    lastNode = node->last;
  }
  node->next = node->last = NULL;
  return true;
}

}

}

}
