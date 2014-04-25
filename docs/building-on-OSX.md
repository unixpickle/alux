# Building on OS X

In order to compile Alux (at least, officially), you must have the a GNU C Linker on your system. Even if you have Xcode and its command-line utilities installed, you probably do not have GNU binutils&ndash;instead, you most likely have llvm sym-linked to `/usr/bin/ld` or to a similar path.

For this reason, you probably need to install the *real* binutils.

On the x86-64 platform, Alux uses NASM to assemble code for initialization and context switching. You will need to install this in addition.

## Building binutils

Download [binutils](http://www.gnu.org/software/binutils/). Then go into the source directory and run the following:

    ./configure --program-prefix=gelf64- --prefix=/usr/local --enable-multilib --enable-ld=yes --target=x86_64-unknown-elf --disable-werror
    make
    make install

Now, you should have the `gelf64-ld` command installed on your machine.

## Installing NASM with Homebrew

Simply run this and you're done!

    brew install nasm

## Building Using a Custom Tools

Finally, you will need to compile and link Alux with your custom toolchain. You can still use the default Clang compiler, you just need to give it some options. To do this, run these commands before running the normal `make` command:

    export LD=your-linker-or-gcc
    export CXXFLAGS=-integrated-as -target x86_64-pc-none-elf64

You may additionally wish to add:

	export NASM=your-nasm

Now, you may simply run `make` in the root project directory, which will generate a `build/` directory. If the compilation is successful, a multiboot-able file `alux.bin` should be created in the `build/` directory.
