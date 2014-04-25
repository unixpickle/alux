OUTPUT_FILE = alux.bin

$(OUTPUT_FILE): $(LIBOS) $(CUSTOM_OUTDIR)
	cp $(CUSTOM_OUTDIR)/x64-multiboot-init.bin $(OUTPUT_FILE)
	$(LINK) $(LIBOS) $(CUSTOM_OUTDIR)/*.o -T linker.ld --oformat binary -s -o $(OUTPUT_FILE).1
	cat $(OUTPUT_FILE).1 >>$(OUTPUT_FILE)
	rm $(OUTPUT_FILE).1
