# Copyright (c) 2014, Alex Nichol and Alux contributors.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice,
#	this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#	this list of conditions and the following disclaimer in the documentation
#	and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

PROJECT_ROOT = $(shell pwd)
TARGET_ARCH ?= x64
BINFILE ?= alux.bin
BUILD_DIR ?= $(PROJECT_ROOT)/build
CUSTOM_OUTDIR ?= $(BUILD_DIR)/custom-out

override CXXFLAGS += -nostdlib -nostdinc -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -Wno-long-long -Wextra -std=c++11

CXX ?= "g++"
CC ?= gcc
LD ?= ld
NASM ?= nasm

export CUSTOM_OUTDIR
export PROJECT_ROOT
export BINFILE
export TARGET_ARCH
export CXX
export CC
export LD
export NASM
export CXXFLAGS

$(BUILD_DIR)/$(BINFILE): $(BUILD_DIR)
	$(MAKE) -C $(BUILD_DIR)/obj
	$(MAKE) -C ./src/custom/$(TARGET_ARCH)
	$(MAKE) -C ./link/$(TARGET_ARCH) OBJS=$(BUILD_DIR)/obj OUTPUT_FILE=$(BUILD_DIR)/$(BINFILE)

$(BUILD_DIR): dependencies
	mkdir $(BUILD_DIR)
	bash generator.sh $(TARGET_ARCH) $(BUILD_DIR)/obj

dependencies:
	mkdir dependencies
	git clone https://github.com/unixpickle/anlock.git dependencies/anlock
	git clone https://github.com/unixpickle/analloc2.git dependencies/analloc2

image: $(BUILD_DIR)/$(BINFILE) $(BUILD_DIR)/grub_root
	grub-mkrescue -o $(BUILD_DIR)/alux.iso $(BUILD_DIR)/grub_root/

$(BUILD_DIR)/grub_root:
	mkdir $(BUILD_DIR)/grub_root
	mkdir $(BUILD_DIR)/grub_root/boot
	mkdir $(BUILD_DIR)/grub_root/boot/grub
	cp $(PROJECT_ROOT)/src/custom/$(TARGET_ARCH)/grub.cfg $(BUILD_DIR)/grub_root/boot/grub
	cp $(BUILD_DIR)/$(BINFILE) $(BUILD_DIR)/grub_root/boot

clean:
	rm -rf build

clean-all: clean
	rm -rf dependencies
