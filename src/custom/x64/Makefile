NASM = nasm
OBJFORMAT = $(shell bash ./native_objformat.sh)

all: out out/multiboot_entry.o out/proc_entry.o

out:
	mkdir out

out/multiboot_entry.o: multiboot_entry.s
	nasm -f $(OBJFORMAT) $^ -o $@

out/proc_entry.o: proc_entry.s
	nasm -f $(OBJFORMAT) $^ -o $@

clean:
	rm -rf out
