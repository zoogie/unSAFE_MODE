#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

NAME := $(notdir $(CURDIR))

SUBFOLDERS := stage1 stage2 stage3_pre9otherapp mini_b9s_installer bb3_installer bb3_installer/payload

.PHONY:    all release clean $(SUBFOLDERS) slotTool

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
	@$(MAKE) -C $@ all
