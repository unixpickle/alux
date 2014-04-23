PLATFORM=x64

all: ./build
	make -C ./build

./build: dependencies
	./dependencies/gyp/gyp --depth=. -f make --generator-output=./build -D PLATFORM=$(PLATFORM) alux.gyp

dependencies:
	mkdir dependencies
	svn checkout http://gyp.googlecode.com/svn/trunk/ dependencies/gyp

clean:
	rm -rf build

clean-all:
	rm -rf dependencies
