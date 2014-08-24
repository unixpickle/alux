# Alux

Alux is a small Hobby Operating System. The purpose of this project is to create a lightweight microkernel on top of which you may place a language interpreter like V8 or the Dart VM.

# Objectives

I will attempt to do all of the following:

 * Provide a minimal, lightweight kernel API for IPC, basic memory management, task scheduling and time management.
 * Provide a thin layer of security specific to the CPU architecture
 * Allow user-space tasks to manage the resources of other user-space tasks with almost no exceptions

As an end result, I hope to be able to create a distribution of Alux that runs the Dart VM. This distribution will ship with a set of drivers and programs written entirely in Dart. The distribution *may* include some sort of user-space ELF loader for native plugins.

Additionally, I plan to spin off a version of Alux that runs the V8 JavaScript engine. Ideally, if [Runtime.JS](https://github.com/runtimejs/runtime) ends up with a strong JavaScript API, I will implement that as part of Alux.js.

## TODO

I am now in the process of making a simple "Hello world" user-space program execute:

 * Create a `UserTask` classes
 * Create a `ProgramImage` class that I initialize at boot
 * Hook up a system call handler that prints "Hello world"
 * Launch a `UserTask` that runs the system call (i.e. a test *libalux* app)

After all that, I will implement system calls. These system calls will provide the following facilities:

   * Threading, mutexes, semaphores
   * Memory management
   * Launching and killing sub-tasks
   * Basic I/O (e.g. keyboard driver, console)
   * Assertions, exit statuses
   * Static variables
   * NX regions of memory

As I implement these system calls, I will create appropriate bindings for [libalux](http://github.com/unixpickle/libalux).

Finally, I plan to work on an IPC mechanism. I still haven't a clue how this will work, but here's the general features it will have:

 * The ability to connect to a task by specifying its PID
 * A means by which to transfer information between the kernel and a driver
 * A shared-memory facility for very efficient inter-process sockets

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
