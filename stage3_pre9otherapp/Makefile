ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

ifeq ($(filter $(DEVKITARM)/bin,$(PATH)),)
export PATH:=$(DEVKITARM)/bin:$(PATH)
endif

DEFINES	:=	-DOTHERAPP=1
LDPATH	:=	otherapp.ld

CC = arm-none-eabi-gcc
# LINK = arm-none-eabi-gcc
LINK = arm-none-eabi-ld
AS = arm-none-eabi-as
OBJCOPY = arm-none-eabi-objcopy
CFLAGS = -Wall -std=c99 -march=armv6 -Os -fno-exceptions $(DEFINES) -Isource/libctru -Isource
LDFLAGS += --script=$(LDPATH) -L"$(DEVKITARM)/arm-none-eabi/lib" -Map=output.map

CFILES = $(wildcard source/libctru/*.c source/*.c source/exploits/*.c source/bins/*.c)
BINFILES = $(wildcard data/*.bin)
OFILES = $(BINFILES:data/%.bin=build/%.bin.o)
OFILES += $(CFILES:source/%.c=build/%.o)
DFILES = $(CFILES:source/%.c=build/%.d)
SFILES = $(wildcard source/libctru/*.s source/exploits/*.s source/bins/*.s source/*.s)
OFILES += $(SFILES:source/%.s=build/%.o)
PROJECTNAME = otherapp
CWD = "$(CURDIR)""

#---------------------------------------------------------------------------------
# canned command sequence for binary data, taken from devkitARM
#---------------------------------------------------------------------------------
define bin2o
	bin2s $< | $(AS) -o $(@)
	echo "extern const u8" `(echo $(<F) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"_end[];" > source/`(echo $(<F) | tr . _)`.h
	echo "extern const u8" `(echo $(<F) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"[];" >> source/`(echo $(<F) | tr . _)`.h
	echo "extern const u32" `(echo $(<F) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`_size";" >> source/`(echo $(<F) | tr . _)`.h
endef

.PHONY:=all

all: $(PROJECTNAME).bin
	
$(PROJECTNAME).bin: $(PROJECTNAME).elf
	$(OBJCOPY) -O binary $< $@

$(PROJECTNAME).elf: $(OFILES)
	$(LINK) $(LDFLAGS) -o $(PROJECTNAME).elf $(filter-out build/crt0.o, $(OFILES)) -lm -lc

clean:
	@rm -f build/*.o build/*.d build/bins/*.d build/bins/*.o build/libctru/*.d build/libctru/*.o build/exploits/*.d build/exploits/*.o
	@rm -f $(PROJECTNAME).elf $(PROJECTNAME).bin
	@echo "all cleaned up !"

-include $(DFILES)

	
build/%.o: source/%.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(OBJCOPY) --remove-section .eh_frame --remove-section .rel.eh_frame --remove-section .rela.eh_frame $@ $@
	@$(CC) -MM $< > build/$*.d

	
build/%.o: source/%.s
	$(CC) -x assembler-with-cpp $(CFLAGS) -c $< -o $@
	$(OBJCOPY) --remove-section .eh_frame --remove-section .rel.eh_frame --remove-section .rela.eh_frame $@ $@
	@$(CC) -MM $< > build/$*.d

build/%.bin.o: data/%.bin
	@echo $(notdir $<)
	@$(bin2o)