# NOTICE

These docs are out of date. I will be updating them once the scheduling system is more complete.

# Scheduler & Task System

The Alux task system and scheduler sit on top of two different parallel layers of abstraction. One of these is the architecture, and the other is the scheduling algorithm.

## Architecture APIs

For different platforms, you will find that context switching is done very differently. For this reason, architecture-specific APIs provide a mechanism for context switching, executing new regions of code, etc.

Several files outline the scheduler-related abstractions that the architecture-specific system must provide:

 * [&lt;arch/general/hardware-thread.hpp&gt;](../src/arch/general/hardware-thread.hpp) - The basic abstraction of a hardware thread (a.k.a. CPU, execution context, etc.). This mainly consists of an API for sleeping/waking CPUs and an API for CPU-local storage.
 * [&lt;arch/general/hardware-thread-list.hpp&gt;](../src/arch/general/hardware-thread-list.hpp) - A list of hardware threads. This list is ordered by priority, so the first CPUs should be used before the last. This allows platforms like AMD64 which support hyperthreading to instruct the scheduler only to use hyperthreads if necessary.
 * [&lt;arch/general/tick-timer.hpp&gt;](../src/arch/general/tick-timer.hpp) - An API for setting timeouts for the scheduler. In particular, the method `SaveAndTick()` saves the state of the current software thread and calls the scheduler's `Tick()` method. This file provides a mechanism for triggering a call to `SaveAndTick()` after a given amount of time.
 * [&lt;arch/general/state.hpp&gt;](../src/arch/general/state.hpp) - A class `State` which represents the state of a task.
 * [&lt;arch/general/user-map.hpp&gt;](../src/arch/general/user-map.hpp) - A class `UserMap` which represents per-task memory mappings.

## Scheduler APIs

The scheduling algorithm should not be set in stone. For this reason, the scheduling algorithm is abstracted into its own API. The scheduler helps to manage power consumption and instructs the architecture-specific code to run different tasks at different times. This is the only responsibility of the scheduler, and thus it is easy to focus on algorithms without worrying about synchronization, context switching, etc.

The scheduler-specific system must provide three headers, `<scheduler-specific/scheduler.hpp>`, `<scheduler-specific/task.hpp>`, and `<scheduler-specific/thread.hpp>`.

`scheduler.hpp` should provide a class that follows this format in the `OS` namespace:

    class Scheduler : public OS::Module {
    public:
      static void InitGlobal();
	  static Scheduler & GetGlobal();
	  
	  void Start(); // @noncritical	  
	  void AddThread(Thread *); // @critical
	  void SetTimeout(uint64_t deadline, bool precise, uint64_t * unlock = NULL); // @critical
	  void SetInfiniteTimeout(uint64_t * unlock = NULL); // @critical
	  bool ClearTimeout(Thread *); // @critical
	  void Resign(); // @critical
	  void ExitThread() OS_NORETURN; // @critical
	  void ExitTask(uint64_t status) OS_NORETURN; // @critical
	  void Tick(); // @critical
	  
	  // ... your fields here ... //
	  
	protected:
	  friend class Task; // so that it may call RemoveThread()
	  void RemoveThread(Thread *); // @critical
      
      // ... your fields here or in a private section ... //
    };

`thread.hpp` should provide a class&ndash;any class&ndash;by the name of `SchedThread` in the `OS` namespace. `task.hpp` should provide a class named `SchedTask` in the `OS` namespace which also may be empty.

The scheduler must allow the operating system to do the following:

* If the operating system wishes to create a thread, it should first retain a task, add the thread to that task, then add the thread to the scheduler, and finally release the task.
* If the operating system decides to terminate a task, it should retain the task, call `Kill()` on that task, and then release the task.
* If the operating system decides to terminate the current task, it should call `ExitTask(status)` on `Scheduler::GetGlobal()` from a critical section. The calling code should hold no references of any sort to the task.
* If the operating system wishes to terminate the current thread, it should call `ExitThread()` on `Scheduler::GetGlobal()` from a critical section. The calling code should hold no references of any sort to the task.
* The scheduler must in some way `Delete()` terminated threads. The `Task` object will automatically push itself to the garbage thread when it is killed.
* In order for `~Task()` to work, a terminated task must be able to delete its killed threads from the scheduler using a protected or public method `RemoveThread()`.

## The `Task` and `Thread` classes

The `Task` class is declared in [&lt;scheduler/general/task.hpp&gt;](../src/scheduler/general/task.hpp) and the `Thread` class is declared in [&lt;scheduler/general/thread.hpp&gt;](../src/scheduler/general/thread.hpp). `Task` is a subclass of `SchedTask`. Similarly, `Thread` is a subclass of `SchedThread`.

### The `Thread` class

A thread contains a state and holds a pointer to its owning task. A `Thread` instance should only be allocated by initializing a subclass or by calling `Thread::NewUser()` or `Thread::NewKernel()`. A `Thread` should only be deleted by calling `aThread->Delete()`. This allocation mechanism allows for threads to be allocated by a Slab or some other form of memory cache.

A thread is initialized with its owning task. The `GetTask()` method returns a thread's owning task. You still need to use `aTask->AddThread(someThread)` to have the relationship go both ways. If you do not add a thread to a task, the thread will not be properly destroyed when the task terminates.

The state of a thread can be obtained through `GetState()`.

### The `Task` class

Task's are way more interesting than threads. In the kernel, a `Task` may be killed at any time by any piece of code. This raises lots of issues: what if a task is allocating memory, or what if it is holding a lock? What if a task is killed while executing important kernel code?

You might think that critical sections are the solution: simply use a critical section wherever you don't want your task to die. But consider this: allocating memory isn't a critical operation, so what if you want to allocate memory in a system call. You can't do it in a critical section, and you sure don't want your task to die while you have a memory lock held or an unreferenced allocated chunk of memory.

My solution is reference counting. There are two different counters: a *retain* count and a *hold* count. When a task is killed, the task's cleanup process will not initiate until both of these counts reach zero. Once a task has been killed, holding it will fail. Retaining it will fail once the hold count is zero.

The task's cleanup code will remove all references to itself before deleting its associated memory, so it is never possible for a task to get deallocated while you attempt to retain or hold it.

So what's the difference between the retain count and the hold count? When a task is held, killing it will not prevent it from being retained until it is unheld. This means that a kernel thread can hold it's running task in order to ensure that it will not be killed mid-execution.

A retain count is not as powerful. The scheduler will retain a task before running a thread that it owns. This ensures that a task will always be retained while one or more of its threads are running. Since the task's cleanup code will only be called once the retain count reaches zero, it will be impossible to reach cleanup code while a task is still running any threads.

So yeah, that's the explanation of reference counts. A task can be retained with `Retain()` and held with `Hold()`&ndash;both of which return `bool`s to tell the caller if the operation was successful. A task can be released and unheld with `Release()` and `Unhold()` respectively. All of these methods are only to be run from a critical section. A task can be killed with a call to `Kill(uint64_t)`, which takes a numerical "kill status."

## The Kernel Task & Garbage Thread

The `KernelThread` and `GlobalThread` modules are necessary throughout the scheduler.

Did you ever wonder how the critical-only `Release()` and `Unhold()` methods could ever possibly cleanup and free a task that's been killed? Well, you should, since deallocation is a noncritical operation.

When a task is killed, it is pushed to the garbage thread, which runs in kernel space and deletes tasks and threads from a queue. This way, the task is asynchronously cleaned up.

## The `HoldScope` class

While the mechanics of hold counts are fairly straight-forward, they can be a pain to implement. The `HoldScope` class uses constructor and destructor side-effects to alleviate this process.

When a `HoldScope` is initialized, it attempts to hold the current task. Furthermore, it enters a non-critical section (if one was not already in place). If the initializer cannot hold the current task, it terminates the current thread. Thus, after initializng a `HoldScope`, you may use your non-critical section knowing that it will not be interrupted due to task termination.

The hold scope also provides some utilities. Say you use a hold scope in a system call, and that system call notices that the calling task has attempted to do something illegal. The proper thing to do is terminate the current task with an error code. To make this easy, you may call the `Exit(uint64_t)` method on a `HoldScope` to terminate the current task and automatically unhold it.

Since the hold scope obtains the current `Task` in order to hold it, it is able to provide a convenient `GetTask()` method. This way, you don't always have to run `HardwareThread::GetThread()->GetTask()`.

Finally, here is an example of all of this together in a print system call:

	HoldScope scope;
  
    UserMap * map = scope.GetTask()->GetUserAddressSpace();
    assert(map != NULL);
    while (1) {
      char str[2] = {0, 0};
      if (!map->CopyToKernel((void *)str, (VirtAddr)strBuf, 1)) {
        scope.Exit(KillReasons::UnownedMemory);
      }
      if (!str[0]) break;
      cout << str;
      strBuf++;
    }
