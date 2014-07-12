# Alux

Alux is a small Hobby Operating System. The purpose of this project is to create a lightweight microkernel on top of which you may place a language interpreter like V8.

General architectual ideas that I am playing around with:

 * A unified address space to reduce latency due to TLB misses
 * CPU pinning for tasks/threads
 * Cross-platform abstractions
 * Memory allocation and VMM completely in user-space

## TODO

I have a test user-space compiling and running. So far, I have created a basic print system call, and some system calls for running and exiting tasks and threads.

**My next step is a somewhat lightweight "rewrite" of a large part of the kernel.** I am planning to move all of the general APIs in the kernel to [ansa](http://github.com/unixpickle/ansa). I am also working to move all of the architecture-specific APIs to [anarch](http://github.com/unixpickle/anarch). Once I have both of these migrations completed, I will continue to work on the TODO list below.

I want to expand the scheduler with an IPC mechanism, and I want to implement some useful syscalls. To do this, I plan to modify the kernel and my tests simultaneously for rapid testing and development.

 * Clean up build system and remove various environment variables
 * Figure out a cleaner way to handle memory faults
 * Write basic `new` and `delete` for user-space
 * Come up with a viable IPC mechanism and implement it
 * Create tests that implement:
   * Threading, mutexes, semaphores
   * Memory management
   * Launching and killing sub-tasks
   * Basic I/O (e.g. keyboard driver, console)
   * Assertions, exit statuses
   * Static variables
   * NX regions of memory

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
