PROJECT_ROOT = $(shell pwd)
TARGET_ARCH ?= x64
BUILDDIR = $(PROJECT_ROOT)/build
OBJDIR = $(BUILDDIR)/objects
#CXX ?= "g++"
#CC ?= gcc
#LD ?= ld
#NASM ?= nasm

export PROJECT_ROOT
export TARGET_ARCH
export BUILDDIR
export OBJDIR
#export CXX
#export CC
#export LD
#export NASM

override CXXFLAGS += -mcmodel=large
override CFLAGS += -mcmodel=large
export CXXFLAGS
export CFLAGS

$(BUILDDIR)/$(OUTFILE):
	mkdir $(BUILDDIR)
	mkdir $(OBJDIR)
	coffee generate.coffee
	$(MAKE) -C $(OBJDIR)
	$(MAKE) -C ./link/$(TARGET_ARCH)

clean:
	rm -rf build
