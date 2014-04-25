NASM ?= nasm
OBJFORMAT ?= $(shell bash ./native_objformat.sh)
BINFORMAT ?= bin
CUSTOM_OUTDIR ?= ./out
OUTDIR := $(CUSTOM_OUTDIR)

all: $(OUTDIR) $(OUTDIR)/x64-proc-init.o $(OUTDIR)/x64-entry.o $(OUTDIR)/x64-multiboot-init.bin

$(OUTDIR):
	mkdir $(OUTDIR)

$(OUTDIR)/x64-proc-init.o: x64-proc-init.s
	$(NASM) -f $(OBJFORMAT) $^ -o $@

$(OUTDIR)/x64-entry.o: x64-entry.s
	$(NASM) -f $(OBJFORMAT) $^ -o $@

$(OUTDIR)/x64-multiboot-init.bin: x64-multiboot-init.s
	$(NASM) -f $(BINFORMAT) $^ -o $@

clean:
	rm -rf $(OUTDIR)
