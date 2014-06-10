# Scheduler & Task System

The Alux task system and scheduler sit on top of two different parallel layers of abstraction. One of these is the architecture, and the other is the scheduling algorithm.

## Architecture APIs

For different platforms, you will find that context switching is done very differently. For this reason, architecture-specific APIs provide a mechanism for context switching, executing new regions of code, etc.

Several files outline the scheduler-related abstractions that the architecture-specific system must provide:

 * [<arch/general/hardware-thread.hpp>](src/arch/general/hardware-thread.hpp) - The basic abstraction of a hardware thread (a.k.a. CPU, execution context, etc.). This mainly consists of an API for sleeping/waking CPUs and an API for CPU-local storage.
 * [<arch/general/hardware-thread-list.hpp>](src/arch/general/hardware-thread-list.hpp) - A list of hardware threads. This list is ordered by priority, so the first CPUs should be used before the last. This allows platforms like AMD64 which support hyperthreading to instruct the scheduler only to use hyperthreads if necessary.
 * [<arch/general/tick-timer.hpp>][src/arch/general/tick-timer.hpp] - An API for setting timeouts for the scheduler. In particular, the method `SaveAndTick()` saves the state of the current software thread and calls the scheduler's `Tick()` method. This file provides a mechanism for triggering a call to `SaveAndTick()` after a given amount of time.

Additionally, when a given platform is linked with the kernel, it must provide headers located in `<arch-specific/thread.hpp>` and `<arch-specific/task.hpp>`.

The file `thread.hpp` must define a class with this format in the `OS` namespace:

    class ArchThread {
    public:
      ArchThread(Task * task, bool forKernel = false); // @noncritical
      void SetKernelCall(void * function); // @ambicritical
      void SetKernelCall(void * function, void * argument); // @ambicriitcal
      void Run(); // @critical -> @noncritical (no return)
      // ... your fields here or in a private section ... //
    };

If `true` is passed as the second initializer argument, your thread instance must allow calls to `SetKernelCall` before it has started running. This method allows the caller to set the entry-point of this thread. In this case, the thread will execute the entry point starting in a noncritical section. The entry function may **not** return to kill the thread; it must terminate the thread manually. The thread's `Run()` method must resume the thread or run it from the initial state if it has not been run before.

You will notice that this API provides no mechanism for configuring user threads. The platform is responsible for creating and managing all user-threads: this allows for flexibility in executable formats and dynamic code loading.

The file `task.hpp` must define a class called `ArchTask` in the `OS` namespace which must follow this format:

    class ArchTask {
    public:
      ArchTask(bool kernel = false); // @noncritical
      // ... your fields here or in a private section ... //
    };

This will create a task that is optionally a kernel task.

## Scheduler APIs

The scheduling algorithm should not be set in stone. For this reason, the scheduling algorithm is abstracted into its own API. The scheduler helps to manage power and instructs the architecture-specific code to run different tasks at different times. This is the only responsibility of the scheduler, and thus it is easy to focus on algorithms without worrying about synchronization, context switching, etc.

The scheduler-specific system must provide three headers, `<scheduler-specific/scheduler.hpp>`, `<scheduler-specific/task.hpp>`, and `<scheduler-specific/thread.hpp>`.

`scheduler.hpp` should provide a class that follows this format in the `OS` namespace:

    class Scheduler {
    public:
      static void Initialize();
      static Scheduler & GetGlobal();
      
      void Start(); // @noncritical
      
      void AddThread(Thread *); // @critical
      void RemoveThread(Thread *); // @critical
      
      void SetTimeout(uint64_t deadline, bool precise); // @critical
      void SetInfiniteTimeout(); // @critical
      bool ClearTimeout(Thread *); // @critical
      void Resign(); // @critical
      
      void Tick(); // @critical
      
      // ... your fields here or in a private section ... //
    };

`thread.hpp` should provide a class&ndash;any class&ndash;by the name of `SchedThread` in the `OS` namespace. `task.hpp` should provide a class named `SchedTask` in the `OS` namespace which also may be empty.

## The `Task` and `Thread` classes

The `Task` class is declared in [<scheduler/general/task.hpp>](src/scheduler/general/task.hpp) and the `Thread` class is declared in [<scheduler/general/thread.hpp>](src/scheduler/general/thread.hpp). `Task` is a subclass of both `ArchTask` and `SchedTask`. Similarly, `Thread` is a subclass of `ArchThread` and `SchedThread`.

### The `Thread` class

A thread contains very little information of its own. A `Thread` instance should only be allocated by initializing a subclass or by calling `Thread::New()`. A `Thread` should only be deleted by calling `aThread->Delete()`. This allocation mechanism allows for threads to be allocated by a Slab or some other form of memory cache.

A thread is initialized with its owning task. The `GetTask()` method returns a thread's owning task.

Besides these features, pretty much all of a `Thread`'s functionality comes from its super-classes. The `ArchThread` super-class facilitates context-switching functionality, and the `SchedThread` super-class manages scheduling information.

### The `Task` class

Task's are way more interesting than threads. In the kernel, a `Task` may be killed at any time by any piece of code. This raises lots of issues: what if a task is allocating memory, or what if it is holding a lock? What if a task is killed while executing important kernel code?

You might think that critical sections are the solution: simply use a critical section wherever you don't want your task to die. But consider this: allocating memory isn't a critical operation, so what if you want to allocate memory in a system call. You can't do it in a critical section, and you sure don't want your task to die while you have a memory lock held or an unreferenced allocated chunk of memory.

The solution&nbsp;my solution&nbsp;is reference counting. There are two different counters: a *retain* count and a *hold* count. When a task is killed, the task cleanup process will not initiate until both of these counts reaches zero. Once a task has been killed, holding it will fail and retaining it will fail once the hold count is zero.

The task's cleanup code will remove all references to itself before deleting its assocatied memory, so it is never possible for a task to get deallocated while attempting to retain or hold it.

So what's the difference between the retain count and the hold count? When a task is held, killing it will have no effect until it is unheld. This means that a kernel thread can hold it's running task in order to ensure that it will not be killed mid-execution.

A retain count is not as powerful. The scheduler will retain a task before running a thread that it owns. This ensures that a task will always be retained while one or more of its threads are running. Since the task's cleanup code will only be called once the retain count reaches zero, it will be impossible to reach cleanup code while a task is still running.

So yeah, that's the explanation of retain counts. A task can be retained with `Retain()` and held with `Hold()`&nbsp;both of which return `bool`s to tell the caller if the operation was successful. A task can be released and unheld with `Release()` and `Unhold()` respectively. All of these methods are only to be run from a critical section. A task can be killed with a call to `Kill(uint64_t)`, which takes a numerical "kill status."

## The Kernel Task & Garbage Thread

Along with initializing the global scheduler, the platform specific code must initialize and run the `KernelTask` and `GarbageThread`.

Did you ever wonder how the critical-only `Release()` and `Unhold()` methods could ever possibly cleanup and free a task that's been killed? Well, you should, since deallocation is a noncritical operation.

When a task is killed, it is pushed to the garbage thread, which runs in kernel space and deletes task's from a queue. This way, the task is asynchronously cleaned up.
