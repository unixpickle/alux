# Building on OS X

In order to compile Alux (at least, officially), you must have the a GNU C Compiler on your system. Even if you have Xcode and its command-line utilities installed, you probably do not have GCC&ndash;instead, you most likely have clang sym-linked to `/usr/bin/gcc` or to a similar path.

For this reason, you probably need to install the *real* GNU C Compiler.

On the x86-64 platform, Alux uses NASM to assemble code for initialization and context switching.

## Installing GCC with Homebrew

First, I recommend looking up the latest version of GCC. As of writing this document, GCC 4.9.0 has just come out, so I will be using that version:

    brew tap homebrew/versions
    brew install gcc49

Remember to change `gcc49` to `gccxy` where GCC version *x.y* is the latest release.

## Installing GCC Manually

First, I'd like to say thanks to the [OSDev wiki](http://wiki.osdev.org/GCC_Cross-Compiler) for supplying these awesome instructions. Here, I will just be summarizing them for you.

First, you will need to snatch up the following dependencies. You should install these on your system:

* [GNU GMP](https://gmplib.org)
* [GNU MPFR](http://mpfr.org)
* [ISL](http://isl.gforge.inria.fr)
* [ClooG](http://www.cloog.org)
* [GNU MPC](http://multiprecision.org)
* [libiconv](https://gnu.org/software/libiconv) (note that this is already installed, just probably *old*)

Next, you should download the latest GCC source tree itself from the [GCC website](https://gnu.org/software/gcc).

## Installing NASM with Homebrew

Simply run this and you're done!

    brew install nasm

## Building Using a Custom Tools

Finally, you will need to compile and link Alux with your custom toolchain. To do this, run these commands before running the normal `make` command:

    export CXX=your-c++-compiler
    export LD=your-linker-or-gcc
    export CC=your-gcc

You may additionally wish to add:

	export NASM=your-nasm

Now, you may simply run `make` in the root project directory, which will generate a `build/` directory. If the compilation is successful, a multiboot-able file `alux.bin` should be created in the `build/` directory.

