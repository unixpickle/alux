OUTPUT_FILE = $(BUILDDIR)/alux.bin
OUTPUT_FORMAT ?= binary
CUSTOM_OUTDIR = $(BUILDDIR)/custom

$(OUTPUT_FILE): $(CUSTOM_OUTDIR)
	$(LD) $(OBJDIR)/*.o $(CUSTOM_OUTDIR)/*.o -T linker.ld --oformat=$(OUTPUT_FORMAT) -o $(OUTPUT_FILE).1
	if [ "$(OUTPUT_FORMAT)" = "binary" ]; then \
		cp $(CUSTOM_OUTDIR)/x64-multiboot-init.bin $(OUTPUT_FILE); \
		cat $(OUTPUT_FILE).1 >>$(OUTPUT_FILE); \
	  rm $(OUTPUT_FILE).1; \
	else \
	  mv $(OUTPUT_FILE).1 $(OUTPUT_FILE).elf; \
	fi
	
