# Scheduler

The Alux task system and scheduler sit on top of two different parallel layers of abstraction. One of these is the architecture, and the other is the scheduling algorithm.

## Architecture APIs

For different platforms, you will find that context switching is done very differently. For this reason, architecture-specific APIs provide a mechanism for context switching, executing new regions of code, etc.

Several files outline the scheduler-related abstractions that the architecture-specific system must provide:

 * [<arch/general/hardware-thread.hpp>](src/arch/general/hardware-thread.hpp) - The basic abstraction of a hardware thread (a.k.a. CPU, execution context, etc.). This mainly consists of an API for sleeping/waking CPUs and an API for CPU-local storage.
 * [<arch/general/hardware-thread-list.hpp>](src/arch/general/hardware-thread-list.hpp) - A list of hardware threads. This list is ordered by priority, so the first CPUs should be used before the last. This allows platforms like AMD64 which support hyperthreading to instruct the scheduler only to use hyperthreads if necessary.
 * [<arch/general/tick-timer.hpp>][src/arch/general/tick-timer.hpp] - An API for setting timeouts for the scheduler. In particular, the method `SaveAndTick()` saves the state of the current software thread and calls the scheduler's `Tick()` method. This file provides a mechanism for triggering a call to `SaveAndTick()` after a given amount of time.

Additionally, when a given platform is linked with the kernel, it must provide headers located in `<arch-specific/thread.hpp>` and `<arch-specific/task.hpp>`.

## Scheduler APIs

The scheduling algorithm should not be set in stone. For this reason, a second layer of abstraction exists in the task system. The scheduler itself helps to manage power and instructs the architecture-specific code to run different tasks at different times. This is the only responsibility of the scheduler, and thus it is easy to focus on algorithms without worrying about synchronization, context switching, etc.
