binary: kernel/build

tests: test/build kernel/build
	coffee set-length.coffee kernel/build/alux.bin test/build/test.bin
	cat test/build/test.bin >>kernel/build/alux.bin

kernel/build: dependencies
	$(MAKE) -C kernel

test/build: dependencies
	$(MAKE) -C test

dependencies:
	mkdir dependencies
	git clone https://github.com/unixpickle/anlock.git dependencies/anlock
	git clone https://github.com/unixpickle/analloc2.git dependencies/analloc2
	git clone https://github.com/unixpickle/anslabs.git dependencies/anslabs
	git clone https://github.com/unixpickle/makemaker.git dependencies/makemaker

clean:
	cd kernel && make clean && cd ..
	cd test && make clean && cd ..

clean-all: clean
	rm -rf dependencies
