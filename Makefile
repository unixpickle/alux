TARGET_ARCH ?= x64
export TARGET_ARCH

all: kernel/build/kernel

kernel/build/kernel:
	$(MAKE) -C kernel

clean:
	$(MAKE) -C kernel clean

clean-all:
	$(MAKE) -C kernel clean-all
