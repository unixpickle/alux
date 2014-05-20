PROJECT_ROOT = $(shell pwd)
TARGET_ARCH ?= x64
BINFILE ?= alux.bin
BUILD_DIR ?= $(PROJECT_ROOT)/build
CUSTOM_OUTDIR ?= $(BUILD_DIR)/custom-out

override CXXFLAGS += -nostdlib -nostdinc -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -Wno-long-long -Wextra -std=c++11 -mno-sse -mno-mmx
override CFLAGS += -nostdlib -nostdinc -ffreestanding -mno-red-zone -Wno-long-long -Wextra -mno-sse -mno-mmx

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
export CFLAGS

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
