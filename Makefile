PROJECT_ROOT = $(shell pwd)
TARGET_ARCH ?= x64
BINFILE ?= alux.bin
BUILD_DIR ?= $(PROJECT_ROOT)/build
CUSTOM_OUTDIR ?= $(BUILD_DIR)/custom-out

export CUSTOM_OUTDIR
export PROJECT_ROOT
export BINFILE
export TARGET_ARCH

$(BUILD_DIR)/$(BINFILE): $(BUILD_DIR)
	$(MAKE) -C ./build
	$(MAKE) -C ./src/custom/$(TARGET_ARCH)
	$(MAKE) -C ./link/$(TARGET_ARCH) LIBOS=$(BUILD_DIR)/out/Default/obj.target/libos.a OUTPUT_FILE=$(BUILD_DIR)/$(BINFILE)

$(BUILD_DIR): dependencies
	./dependencies/gyp/gyp --depth=. -f make --generator-output=$(BUILD_DIR) -D PLATFORM=$(TARGET_ARCH) alux.gyp

dependencies:
	mkdir dependencies
	svn checkout http://gyp.googlecode.com/svn/trunk/ dependencies/gyp

image: $(BUILD_DIR)/$(BINFILE) $(BUILD_DIR)/grub_root
	grub-mkrescue -o $(BUILD_DIR)/alux.iso $(BUILD_DIR)/grub_root

$(BUILD_DIR)/grub_root:
	mkdir $(BUILD_DIR)/grub_root
	mkdir $(BUILD_DIR)/grub_root/boot
	mkdir $(BUILD_DIR)/grub_root/boot/grub
	cp $(PROJECT_ROOT)/src/custom/$(TARGET_ARCH)/grub.cfg $(BUILD_DIR)/grub_root/boot/grub

clean:
	rm -rf build

clean-all:
	rm -rf dependencies
