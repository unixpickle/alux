OUTPUT_FILE = $(BUILDDIR)/test.bin
OUTPUT_FORMAT ?= binary

$(OUTPUT_FILE): $(BUILDDIR)/entry.o
	$(LD) $(BUILDDIR)/entry.o $(OBJDIR)/*.o -T linker.ld --oformat=$(OUTPUT_FORMAT) -o $(OUTPUT_FILE)
	if [ "$(OUTPUT_FORMAT)" != "binary" ]; then \
	  mv $(OUTPUT_FILE) $(OUTPUT_FILE).elf; \
	fi

$(BUILDDIR)/entry.o:
	$(ASM) $(ASMFLAGS) entry.s -o $(BUILDDIR)/entry.o
