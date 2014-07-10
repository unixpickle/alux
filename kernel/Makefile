PROJECT_ROOT = $(shell pwd)
TARGET_ARCH ?= x64
TARGET_SCHEDULER ?= fifo
OUTFILE ?= alux.bin
BUILDDIR ?= $(PROJECT_ROOT)/build
OBJDIR ?= $(BUILDDIR)/objects
CXX ?= "g++"
CC ?= gcc
LD ?= ld
ASM ?= nasm

export PROJECT_ROOT
export TARGET_ARCH
export TARGET_SCHEDULER
export OUTFILE
export BUILDDIR
export OBJDIR
export CXX
export CC
export LD
export ASM

override CXXFLAGS += -nostdlib -nostdinc -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -Wno-long-long -Wall -Wextra -std=c++11 -mno-sse -mno-mmx
override CFLAGS += -nostdlib -nostdinc -ffreestanding -mno-red-zone -Wno-long-long -Wall -Wextra -mno-sse -mno-mmx
override ASMFLAGS += -f elf64

export CXXFLAGS
export CFLAGS
export ASMFLAGS

$(BUILDDIR)/$(OUTFILE): $(BUILDDIR)
	$(MAKE) -C $(OBJDIR)
	$(MAKE) -C ./src/custom/$(TARGET_ARCH)
	$(MAKE) -C ./link/$(TARGET_ARCH)

$(BUILDDIR):
	mkdir $(BUILDDIR)
	mkdir $(OBJDIR)
	../dependencies/makemaker/bin/makemaker build.coffee

clean:
	rm -rf build
