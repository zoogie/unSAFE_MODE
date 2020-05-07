#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

NAME := $(notdir $(CURDIR))

SUBFOLDERS := stage1 stage2 stage3_universal_otherapp mini_b9s_installer bb3_installer bb3_installer/payload bb3_installer/payload/otherapp_template
MAKEOPTS :=

.PHONY: all release clean $(SUBFOLDERS) slotTool

all: usm.bin slotTool

clean:
	@$(MAKE) -C slotTool clean
	@$(foreach dir, $(SUBFOLDERS), $(MAKE) -C $(dir) clean &&) true
	@rm -rf usm.bin
	@echo clean ...

usm.bin: $(SUBFOLDERS) build_payload.py
	@python build_payload.py
	@echo built ... $(notdir $@)

# If the output slot1.bin is the same, don't rebuild slotTool
slotTool: $(SUBFOLDERS)
	@$(MAKE) -C $@ all

$(SUBFOLDERS):
	@$(MAKE) -C $@ $(MAKEOPTS) all

stage3_universal_otherapp: MAKEOPTS += DEFAULT_PAYLOAD_FILE_OFFSET=0x10000 DEFAULT_PAYLOAD_FILE_NAME="usm.bin" MEMCHUNKHAX_ONLY=1
