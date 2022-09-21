ifeq ($(CONFIG_SOC_AMD_COMMON),y)
subdirs-y += block
subdirs-y += fsp
subdirs-y += pi
subdirs-y += vboot
endif

ifneq ($(CONFIG_AMDFW_CONFIG_FILE), )
FIRMWARE_LOCATION=$(shell grep -e FIRMWARE_LOCATION $(CONFIG_AMDFW_CONFIG_FILE) | awk '{print $$2}')

# Add all the files listed in the config file
POUND_SIGN=$(call strip_quotes, "\#")
DEP_FILES= $(patsubst %,$(FIRMWARE_LOCATION)/%, $(shell sed -e /^$(POUND_SIGN)/d -e /*/d -e /^FIRMWARE_LOCATION/d $(CONFIG_AMDFW_CONFIG_FILE) | awk '{print $$2}' ))

amd_microcode_bins += $(wildcard ${FIRMWARE_LOCATION}/*UcodePatch_*.bin)

ifeq ($(CONFIG_RESET_VECTOR_IN_RAM),y)
$(objcbfs)/bootblock.bin: $(obj)/amdfw.rom
	cp $< $@

add_bootblock = \
	$(CBFSTOOL) $(1) add -f $(2) -n apu/amdfw -t amdfw \
	-b $(call int-add, \
	$(call int-subtract, 0xffffffff \
	$(call int-shift-left, \
	0x80000 $(CONFIG_AMD_FWM_POSITION_INDEX))) 0x20000 1)
endif # ifeq ($(CONFIG_RESET_VECTOR_IN_RAM),y)

ifeq ($(CONFIG_VBOOT_GSCVD),y)
build_complete:: $(AMDFWREAD)
endif # ifeq ($(CONFIG_VBOOT_GSCVD),y)

endif
