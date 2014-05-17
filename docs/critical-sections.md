# Basic Explanation

In various cases, you may wish to lock yourself to a certain CPU inside the kernel. For example, if you have just changed a memory map, you should lock yourself to the current CPU in order to invalidate it's page cache. Additionally, if you ever intend to get task information from the current CPU's data structures, you better lock yourself to *that* CPU. Sections of code that are re-entrant are called **noncritical sections**. Conversly, non-interruptable code is called a **critical section**.

## Examples of noncritical sections

Here is a brief list of tasks which are noncritical:

 * Allocating physical memory
 * Allocating virtual kernel memory

## Examples of critical sections

 * Invalidating page cache
 * Sending an inter-processor interrupt

# Entering & Leaving Critical Sections

## The barebones way

The file `platform/interrupts.hpp` contains the two functions:

    bool GetInterruptsEnabled();
    void SetInterruptsEnabled(bool flag);

You may use `SetInterruptsEnabled(false)` to enter a critical section, and `SetInterruptsEnabled(true)` to leave a critical section.

## Scoped critical sections

A lot of the time, you want a given method or scope to run in a critical section. To achieve this, you may use the `ScopeCritical` class defined in `utilities/critical.hpp`. The `ScopeCritical` initializer saves the current interrupt mask and then disables interrupts. The destructor restores the old interrupt mask. This way, you may nest `ScopeCriticals` as much as you like.

# Avoiding Criticality-Related Bugs

## Mark your methods

Whenever you declare a method in the kernel that is to be called after the scheduler has been initailized, you should mark whether it is critical or noncritical. You can do this with a comment:

    /**
     * Print "foobar"
     * @noncritical
     */
    void PrintFoobar();
    
    /**
     * Make the CPU explode.
     * @critical
     */
    void InstallWindows();
    
    /**
     * Find the sum of two numbers.
     * @ambicritical
     */
    int AddNumbers(int a, int b);
    
    /**
     * Handle a page fault interrupt.
     * @critical -> @noncritical -> @critical
     */
    void FaultInterrupt();
    
    /**
     * Map in a page of physical memory.
     * @noncritical -> @critical -> @noncritical
     */
    VirtAddr MapPage(PhysAddr page);

The first two examples are pretty straight forward. The third is a function which does not care about the criticality of its caller. Many methods will be **ambicritical**, since they may do simple arithmetic or processing that requires no external OS features.

Note the last two examples. These are functions that switch criticality. The first one, `FaultInterrupt()`, should be called by a critical function, but may perform any number of task switches while it runs. The second, `MapPage()`, should be called from a noncritical section, but may lock the CPU for periods of time during its execution. It would also be appropriate to mark `MapPage()` as noncritical, because a caller should not care whether a function will lock the CPU.

## Assert criticality

Many bugs may arise in the kernel if a critical method is being called from a noncritical method or vice versa. The file `utilities/critical.hpp` contains two macros, `AssertCritical()` and `AssertNoncritical()` which you may invoke like functions in order to assert criticality at the beginning of your functions.
