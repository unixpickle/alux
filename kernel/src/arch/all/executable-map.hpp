#ifndef __ALUX_EXECUTABLE_MAP_HPP__
#define __ALUX_EXECUTABLE_MAP_HPP__

namespace Alux {

class ExecutableMap {
public:
  /**
   * Returns the executable this map was created from.
   * @ambicritical
   */
  virtual Executable & GetExecutable() = 0;
  
  /**
   * Returns the address of the programs main function
   * @ambicritical
   */
  virtual void * GetEntryPoint() = 0;
  
  /**
   * While a program is running, it may not have all of its code mapped into
   * its address space. Furthermore, some of this code may be mapped read-only
   * while other parts may be mapped to writable regions of memory.
   *
   * The kernel's page fault handler should call [HandlePageFault] on the
   * current executable (if one is running) to see if the page fault occured
   * because of unmapped code.
   *
   * Returns `true` if the memory belongs to the program, `false` otherwise. If
   * `true` is returned, the memory should now be mapped into the task's
   * address space.
   *
   * @noncritical
   */
  virtual bool HandlePageFault(VirtAddr addr, bool write) = 0;
  
  /**
   * Delete the executable map. This does not unmap the executable from the
   * address space, but it may free the physical memory that backs the program.
   * For this reason, you should only call [Delete] once the task has program
   * has finished running.
   * @noncritical
   */
  virtual void Delete() = 0;
  
  /**
   * Returns the user map that this executable map controls.
   */
  inline anarch::UserMap & GetMap() const {
    return map;
  }
  
protected:
  ExecutableMap(anarch::UserMap & m) : map(m) {}
  
  anarch::UserMap & map;
};

}

#endif
