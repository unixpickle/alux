#include <multitasking/task.hpp>
#include <multitasking/task-group.hpp>
#include <arch/general/cpu.hpp>

namespace OS {

Task::Task(TaskGroup * _group) : group(_group) {
}

TaskGroup * Task::GetTaskGroup() {
  return group;
}

Scheduler::JobGroup * Task::GetJobGroup() {
  return static_cast<Scheduler::JobGroup *>(group);
}

void Task::Run() {
  CPU::GetCurrent().SetTask(this);
}

void Task::Cleanup() {
}

}
