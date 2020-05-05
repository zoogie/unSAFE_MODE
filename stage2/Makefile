#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITARM)/base_tools

HAXNAME := LoadCodeMset

.PHONY: all clean

all:	$(HAXNAME).dat

clean:
	@rm -f $(HAXNAME).elf $(HAXNAME).dat
	@echo clean...

$(HAXNAME).dat: $(HAXNAME).elf
	$(OBJCOPY) -O binary $(HAXNAME).elf $(HAXNAME).dat
	@echo built ... $(notdir $@)

$(HAXNAME).elf:	$(HAXNAME).s
	$(CC) -x assembler-with-cpp -nostartfiles -nostdlib -Ttext=0x00600000 $< -o $(HAXNAME).elf
