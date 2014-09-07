override BUILD=../../build
override KERNEL=../../build/kernel

$(KERNEL): $(BUILD)/custom
	$(LD) $(BUILD)/custom/*.o $(BUILD)/objects/*.o -T linker.ld --oformat=$(OUTPUT_FORMAT) -o $(KERNEL).1
	if [ "$(OUTPUT_FORMAT)" = "binary" ]; then \
		cp $(BUILD)/custom/x64-multiboot-init.bin $(KERNEL); \
		cat $(KERNEL).1 >>$(KERNEL); \
		./write-size $(KERNEL); \
	  rm $(KERNEL).1; \
	else \
	  mv $(KERNEL).1 $(KERNEL).elf; \
	fi
