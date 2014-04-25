TARGET_ARCH = x64
BINFILE = alux.bin
PROJECT_ROOT = $(shell pwd)
CUSTOM_OUTDIR = $(PROJECT_ROOT)/build/custom-out

export CUSTOM_OUTDIR
export PROJECT_ROOT
export BINFILE
export TARGET_ARCH

all: ./build
	$(MAKE) -C ./build
	$(MAKE) -C ./src/custom/$(TARGET_ARCH)
	$(MAKE) -C ./link/$(TARGET_ARCH) LIBOS=$(PROJECT_ROOT)/build/out/Default/libos.a OUTPUT_FILE=$(PROJECT_ROOT)/build/$(BINFILE)

./build: dependencies
	./dependencies/gyp/gyp --depth=. -f make --generator-output=./build -D PLATFORM=$(TARGET_ARCH) alux.gyp

dependencies:
	mkdir dependencies
	svn checkout http://gyp.googlecode.com/svn/trunk/ dependencies/gyp

clean:
	rm -rf build

clean-all:
	rm -rf dependencies
