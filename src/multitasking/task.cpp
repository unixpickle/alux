#include <multitasking/task.hpp>
#include <multitasking/task-group.hpp>

namespace OS {

Task::Task(TaskGroup * _group) : group(_group) {
}

TaskGroup * Task::GetTaskGroup() {
  return group;
}

Scheduler::JobGroup * Task::GetJobGroup() {
  return static_cast<Scheduler::JobGroup *>(group);
}

void Task::Cleanup() {
}

}
