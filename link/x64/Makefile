OUTPUT_FILE = alux.bin
OUTPUT_FORMAT ?= binary

$(OUTPUT_FILE): $(LIBOS) $(CUSTOM_OUTDIR)
	cp $(CUSTOM_OUTDIR)/x64-multiboot-init.bin $(OUTPUT_FILE)
	$(LD) $(OBJS)/*.o $(CUSTOM_OUTDIR)/*.o -T linker.ld --oformat=$(OUTPUT_FORMAT) -o $(OUTPUT_FILE).1
	cat $(OUTPUT_FILE).1 >>$(OUTPUT_FILE)
	if [ "$(OUTPUT_FORMAT)" = "binary" ]; then \
	  rm $(OUTPUT_FILE).1; \
	else \
	  mv $(OUTPUT_FILE).1 $(OUTPUT_FILE).elf; \
	fi
	
