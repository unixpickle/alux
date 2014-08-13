# Building on OS X

Alux's individual C and C++ source files can all be compiled with Clang or GCC. However, if you are using OS X, you will probably run into some difficulty linking the final binary.

### Overview

In order to link Alux, you need the GNU Linker (ld) and the GNU Library Archiver (ar). Even if you have Xcode and its command-line utilities installed, you probably do not have GNU binutils&ndash;instead, you most likely have a simpler linker symbolically linked to `/usr/bin/ld` or to a similar path. For this reason, you should install the *real* binutils as described below. Note that the binutils package on Homebrew does *not* include `ld` and is thus insufficient.

On the x86-64 platform, Alux uses NASM to assemble code for initialization and context switching. You will need to install this in addition.

Also, several build scripts use CoffeeScript, so you will need Node and CoffeeScript installed on your system.

## Building binutils

Download [binutils](http://www.gnu.org/software/binutils/). Then go into the source directory and run the following:

    ./configure --program-prefix=x86_64-unknown-elf- --prefix=/usr/local --enable-multilib --enable-ld=yes --target=x86_64-unknown-elf --disable-werror
    make
    make install

Now, you should have the `x86_64-unknown-elf-ld` command installed on your machine.

## Installing NASM with Homebrew

Simply run this and you're done!

    brew install nasm

## Building Alux using Clang on x86-64

The [tools/](../tools/) directory includes a script called `clang-build-x64`. This tool takes two command line arguments: an `ld` command, and an `ar` command. If you followed the above steps to setup GNU binutils, you would run:

    ./tools/clang-build-x64 x86_64-unknown-elf-ld x86_64-unknown-elf-ar

This command will create a multiboot-compliant flat binary in a `build/` directory.

## Building Alux using a custom toolchain

The `LD`, `AR`, `CXX`, `CC`, and `NASM` environment variables give you complete control over the build process.
