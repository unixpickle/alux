PROJECT_ROOT = $(shell pwd)
TARGET_ARCH ?= x64
BUILDDIR = $(PROJECT_ROOT)/build
OBJDIR = $(BUILDDIR)/objects
CXX ?= "g++"
CC ?= gcc
LD ?= ld
NASM ?= nasm

export PROJECT_ROOT
export TARGET_ARCH
export BUILDDIR
export OBJDIR
export CXX
export CC
export LD
export NASM

override CXXFLAGS += -nostdlib -nostdinc -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -Wno-long-long -Wextra -std=c++11 -mno-sse -mno-mmx -mcmodel=large
override CFLAGS += -nostdlib -nostdinc -ffreestanding -mno-red-zone -Wno-long-long -Wextra -mno-sse -mno-mmx -mcmodel=large
override NASMFLAGS += -f elf64

export CXXFLAGS
export CFLAGS
export NASMFLAGS

$(BUILDDIR)/$(OUTFILE):
	mkdir $(BUILDDIR)
	mkdir $(OBJDIR)
	coffee generate.coffee
	$(MAKE) -C $(OBJDIR)
	$(MAKE) -C ./link/$(TARGET_ARCH)

clean:
	rm -rf build
