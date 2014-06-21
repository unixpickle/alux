PROJECT_ROOT = $(shell pwd)
TARGET_ARCH ?= x64
TARGET_SCHEDULER ?= fifo
OUTFILE ?= alux.bin
BUILDDIR ?= $(PROJECT_ROOT)/build
OBJDIR ?= $(BUILDDIR)/objects
CXX ?= "g++"
CC ?= gcc
LD ?= ld
NASM ?= nasm

export PROJECT_ROOT
export TARGET_ARCH
export TARGET_SCHEDULER
export OUTFILE
export BUILDDIR
export OBJDIR
export CXX
export CC
export LD
export NASM

override CXXFLAGS += -nostdlib -nostdinc -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -Wno-long-long -Wextra -std=c++11 -mno-sse -mno-mmx
override CFLAGS += -nostdlib -nostdinc -ffreestanding -mno-red-zone -Wno-long-long -Wextra -mno-sse -mno-mmx
override NASMFLAGS += -f elf64

export CXXFLAGS
export CFLAGS
export NASMFLAGS

$(BUILDDIR)/$(OUTFILE): $(BUILDDIR)
	$(MAKE) -C $(OBJDIR)
	$(MAKE) -C ./src/custom/$(TARGET_ARCH)
	$(MAKE) -C ./link/$(TARGET_ARCH)

$(BUILDDIR): dependencies
	mkdir $(BUILDDIR)
	mkdir $(OBJDIR)
	coffee generate.coffee

test: $(BUILDDIR)/$(OUTFILE)
	$(MAKE) -C test
	coffee set-length.coffee $(BUILDDIR)/$(OUTFILE) test/build/test.bin
	cat test/build/test.bin >>$(BUILDDIR)/$(OUTFILE)

dependencies:
	mkdir dependencies
	git clone https://github.com/unixpickle/anlock.git dependencies/anlock
	git clone https://github.com/unixpickle/analloc2.git dependencies/analloc2
	git clone https://github.com/unixpickle/anslabs.git dependencies/anslabs

image: $(BUILDDIR)/grub_root
	grub-mkrescue -o $(BUILDDIR)/`basename $(OUTFILE) .bin`.iso $(BUILDDIR)/grub_root/

$(BUILDDIR)/grub_root: $(BUILDDIR)/$(OUTFILE)
	mkdir $(BUILDDIR)/grub_root
	mkdir $(BUILDDIR)/grub_root/boot
	mkdir $(BUILDDIR)/grub_root/boot/grub
	cp $(PROJECT_ROOT)/src/custom/$(TARGET_ARCH)/grub.cfg $(BUILDDIR)/grub_root/boot/grub
	cp $(BUILDDIR)/$(OUTFILE) $(BUILDDIR)/grub_root/boot

clean:
	rm -rf build
	cd test && make clean && cd ..

clean-all: clean
	rm -rf dependencies
