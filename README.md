# Alux

Alux is a small Hobby Operating System. The purpose of this project is to create a lightweight microkernel on top of which you may place a language interpreter like V8.

General architectual ideas that I am playing around with:

 * A unified address space to reduce latency due to TLB misses
 * CPU pinning for tasks/threads
 * Cross-platform abstractions
 * Memory allocation and VMM completely in user-space

## TODO

 * Test MapCreator with segmented initial physical memory regions to verify that it adds addresses correctly.

Here's the way I am going to make the kernel map work:

 * Initially, figure out how much memory is needed for the kernel code, and make a topological map of the physical address space.
 * Generate the page tables covering the kernel using the identity-mapped lower GiB of physical memory.
 * Additionally, map in `n` extra page tables, where n is probably 1 or 2 on systems with many CPUs.
 * Switch to the new address space. Note that none of the page tables we just used to create our map are probably mapped in except for the `n` scratch PTs.
 * Now, allow a higher-level object to map in the physical memory needed to represent the physical memory buddy allocator bitmaps. We can modify the physical page tables using our `n` scratch PTs.
 * Use a higher-level object to generate the physical allocators; this object will provide an API for allocating physical memory. 
 * This object will report the last physical address used to the *next* higher up object, which will track mapped regions of kernel virtual memory.

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
