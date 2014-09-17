PROJECT_ROOT=$(shell pwd)
export PROJECT_ROOT

OUTPUT_FORMAT ?= binary
export OUTPUT_FORMAT

override OBJDIR=build/objects
export OBJDIR

CXX ?= g++
CC ?= gcc
LD ?= ld
ASM ?= nasm

export CXX
export CC
export LD
export ASM

CXXFLAGS += -nostdlib -nostdinc -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -Wno-long-long -Wall -Wextra -std=c++11 -mno-sse -mno-mmx -fno-stack-protector -fno-builtin
CFLAGS += -nostdlib -nostdinc -ffreestanding -mno-red-zone -Wno-long-long -Wall -Wextra -mno-sse -mno-mmx -fno-stack-protector -fno-builtin
ASMFLAGS += -f elf64

export CXXFLAGS
export CFLAGS
export ASMFLAGS

build/kernel: build/objects
	$(MAKE) -C src/custom/$(TARGET_ARCH)
	$(MAKE) -C link/$(TARGET_ARCH)

build/objects: build dependencies/anarch/dependencies
	mkdir build/objects
	dependencies/makemaker/bin/makemaker build.coffee
	$(MAKE) -C build/objects

dependencies/anarch/dependencies: dependencies
	$(MAKE) -C dependencies/anarch dependencies

dependencies:
	mkdir dependencies
	git clone --branch v0.0.2 https://github.com/unixpickle/anarch.git dependencies/anarch 2>/dev/null
	git clone --branch v0.0.1 https://github.com/unixpickle/anidmap.git dependencies/anidmap 2>/dev/null
	git clone https://github.com/unixpickle/makemaker.git dependencies/makemaker

build:
	mkdir build

clean:
	rm -rf build
	rm -rf dependencies/anarch/build

clean-all:
	rm -rf build
	rm -rf dependencies
