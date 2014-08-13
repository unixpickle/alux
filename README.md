# Alux

Alux is a small Hobby Operating System. The purpose of this project is to create a lightweight microkernel on top of which you may place a language interpreter like V8 or the Dart VM.

# Objectives

I will attempt to do all of the following:

 * Provide a minimal, lightweight kernel API
 * Provide a thin layer of security specific to the CPU architecture
 * Allow user-space tasks to manage the memory and resources of other user-space tasks with almost no exceptions

As an end result, I hope to be able to create a distribution of Alux that runs the Dart VM. This distribution will ship with a set of drivers and programs written entirely in Dart. The distribution *may* include some sort of user-space ELF loader for native plugins.

## TODO

I have moved architecture-specific code to [anarch](http://github.com/unixpickle/anarch). I am now in the process of implementing a scheduler that sits on top of anarch and performs all of the tasks of the old scheduler:

 * Create a class for a user-space task
 * Come up with a good system for embedding an executable in the kernel binary
 * Write a memory fault handler that copies user-space code
 * Setup the syscall interface for a basic `print` syscall
 * Run a user-space "hello world" program

After this point, some stuff needs to be cleaned up:

 * Unified TaskIdTable class with a hash-map based implementation.
 * A memory-cache API both in anarch and Alux
 * A better identifier table for thread IDs.
 * A clean way for the garbage thread to depend on the scheduler

After all that, I will implement system calls and corresponding user-space tests. These system calls will provide the following facilities:

   * Threading, mutexes, semaphores
   * Memory management
   * Launching and killing sub-tasks
   * Basic I/O (e.g. keyboard driver, console)
   * Assertions, exit statuses
   * Static variables
   * NX regions of memory

Finally, I plan to work on an IPC mechanism. I still have no idea what I will do for this, so I won't even attempt to make a TODO list for it.

## License

Alux is licensed under the BSD 2-clause license. See [LICENSE](https://github.com/unixpickle/alux/blob/master/LICENSE).

```
Copyright (c) 2014, Alex Nichol and Alux contributors.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```
