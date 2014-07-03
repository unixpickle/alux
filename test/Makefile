PROJECT_ROOT = $(shell pwd)
TARGET_ARCH ?= x64
BUILDDIR = $(PROJECT_ROOT)/build
OBJDIR = $(BUILDDIR)/objects
CXX ?= "g++"
CC ?= gcc
LD ?= ld
ASM ?= nasm

export PROJECT_ROOT
export TARGET_ARCH
export BUILDDIR
export OBJDIR
export CXX
export CC
export LD
export ASM

override CXXFLAGS += -nostdlib -nostdinc -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -Wno-long-long -Wextra -Wall -std=c++11 -mno-sse -mno-mmx -mcmodel=large
override CFLAGS += -nostdlib -nostdinc -ffreestanding -mno-red-zone -Wno-long-long -Wextra -Wall -mno-sse -mno-mmx -mcmodel=large
override ASMFLAGS += -f elf64

export CXXFLAGS
export CFLAGS
export ASMFLAGS

all:
	mkdir $(BUILDDIR)
	mkdir $(OBJDIR)
	../dependencies/makemaker/bin/makemaker build.coffee
	$(MAKE) -C $(OBJDIR)
	$(MAKE) -C ./link/$(TARGET_ARCH)

clean:
	rm -rf build
