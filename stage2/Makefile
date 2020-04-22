HAXNAME := LoadCodeMset

all:	$(HAXNAME).dat

clean:
	rm -f $(HAXNAME).elf $(HAXNAME).dat

$(HAXNAME).dat: $(HAXNAME).elf
	arm-none-eabi-objcopy -O binary $(HAXNAME).elf $(HAXNAME).dat

$(HAXNAME).elf:	$(HAXNAME).s
	arm-none-eabi-gcc -x assembler-with-cpp -nostartfiles -nostdlib -Ttext=0x00600000 $< -o $(HAXNAME).elf