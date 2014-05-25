BINFORMAT ?= bin
OUTDIR ?= $(BUILDDIR)/custom

all: $(OUTDIR) $(OUTDIR)/x64-entry.o $(OUTDIR)/x64-multiboot-init.bin

$(OUTDIR):
	mkdir $(OUTDIR)

$(OUTDIR)/x64-entry.o: x64-entry.s
	$(NASM) -f elf64 $^ -o $@

$(OUTDIR)/x64-multiboot-init.bin: x64-multiboot-init.s
	$(NASM) -f bin $^ -o $@

clean:
	rm -rf $(OUTDIR)
