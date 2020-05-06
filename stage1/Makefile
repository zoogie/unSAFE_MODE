#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITARM)/base_tools

UMSDIR		:=	$(CURDIR)/..

REGION := USA
ifeq ($(strip $(REGION)),)
$(error "Set the REGION Makefile parameter.")
endif

HAXID	:=	0
HAXNAME := rop_payload



REGCODE	:=	00
ifeq ($(REGION),USA)
	REGCODE	:=	45
endif
ifeq ($(REGION),EUR)
	REGCODE	:=	56
endif
ifeq ($(REGCODE),00)
	$(error "Unsupported region.")
endif

.PHONY: all clean

all:	slot1.bin

clean:
	@rm -f $(HAXNAME).elf rop_payload.bin slot1.bin
	@echo clean ...

slot1.bin: $(HAXNAME).bin $(UMSDIR)/stage0_unsafemode.py
	@python $(UMSDIR)/stage0_unsafemode.py
	@echo built ... $(notdir $@)

$(HAXNAME).bin: $(HAXNAME).elf
	$(OBJCOPY) -O binary $(HAXNAME).elf rop_payload.bin
	@echo built ... $(notdir $@)

$(HAXNAME).elf:	$(HAXNAME).s
	$(CC) -x assembler-with-cpp -nostartfiles -nostdlib -Ttext=0x00276000 $< -o $(HAXNAME).elf

