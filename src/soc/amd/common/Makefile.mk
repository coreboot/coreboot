## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON),y)
subdirs-y += block
subdirs-y += fsp
subdirs-y += pi
subdirs-y += vboot

CPPFLAGS_common += -I$(src)/soc/amd/common/vboot/include

ifneq ($(V),)
OPT_DEBUG_AMDFWTOOL = --debug
endif

ifneq ($(CONFIG_AMDFW_CONFIG_FILE), )
FIRMWARE_LOCATION=$(shell grep -e FIRMWARE_LOCATION $(CONFIG_AMDFW_CONFIG_FILE) | awk '{print $$2}')

# Add all the files listed in the config file to the dependency list
POUND_SIGN=$(call strip_quotes, "\#")
REMOVE_MP2 =
# If the MP2 file is not being loaded, remove it as a dependency
ifneq ($(CONFIG_PSP_LOAD_MP2_FW),y)
REMOVE_MP2 = /MP2/d
endif

# Steps below to generate the dependency list
# 1a: Delete any line that starts with #, FIRMWARE_LOCATION, SOC_NAME, or AMD_FUSE_CHAIN
# 1b: Filter out lines containing MP2 or not, decided above.
# 1c: Use awk to return only field 2, the filename
# 2: Gather filenames with no path to add the firmware location later
# 3: Gather filenames with a path
# 4a: Add the firmware location to any files without a path to the dependencies
# 4b: add the lines containing a path to the dependencies
AMDFW_CFG_FILES = $(shell sed "/^$(POUND_SIGN)\|^FIRMWARE_LOCATION\|^AMD_FUSE_CHAIN\|^SOC_NAME/d; \
	$(REMOVE_MP2)" $(CONFIG_AMDFW_CONFIG_FILE) | \
	awk '{print $$2}')
AMDFW_CFG_IN_FW_LOC = $(shell echo "$(AMDFW_CFG_FILES)" | tr ' ' '\n' | grep -v "/")
AMDFW_CFG_WITH_PATH = $(shell echo "$(AMDFW_CFG_FILES)" | tr ' ' '\n' | grep "/")
DEP_FILES = $(patsubst %,$(FIRMWARE_LOCATION)/%, $(AMDFW_CFG_IN_FW_LOC)) \
	$(AMDFW_CFG_WITH_PATH)

ifeq ($(CONFIG_RESET_VECTOR_IN_RAM),y)
$(objcbfs)/bootblock.bin: $(obj)/amdfw.rom $(obj)/fmap_config.h
	cp $< $@

amdfw_region_start=$(subst $(spc),,FMAP_SECTION_$(call regions-for-file,apu/amdfw)_START)
amdfw_offset=$(call int-subtract, \
	$(CONFIG_AMD_FWM_POSITION) \
	$(call get_fmap_value,$(amdfw_region_start)))

add_bootblock = \
	$(CBFSTOOL) $(1) add -f $(2) -n apu/amdfw -t amdfw \
        -b $(amdfw_offset) -r $(call regions-for-file,apu/amdfw) \
	$(CBFSTOOL_ADD_CMD_OPTIONS)

endif # ifeq ($(CONFIG_RESET_VECTOR_IN_RAM),y)

ifeq ($(CONFIG_VBOOT_GSCVD),y)
build_complete:: $(obj)/ro-amdfw-list

$(obj)/ro-amdfw-list: $(AMDFWREAD)
	$(AMDFWREAD) --ro-list $(obj)/coreboot.rom > $@

amdfwread-offset-size-cmd = grep '$(1)' $(obj)/ro-amdfw-list | \
				sed 's/^.* 0x0*\(.*\) 0x0*\(.*\)$$/\1:\2/'

amdfwread-range-cmd = $(shell ( \
	range=$$($(call amdfwread-offset-size-cmd,$(1))) ;\
	if [ -n "$$range" ]; then \
		printf $$range ;\
	else \
		printf "error" ;\
	fi ;\
))
endif # ifeq ($(CONFIG_VBOOT_GSCVD),y)

endif # ifneq ($(CONFIG_AMDFW_CONFIG_FILE), )

MAINBOARD_BLOBS_DIR := $(call strip_quotes, $(CONFIG_APCB_BLOBS_DIR))

PHONY+=warn_no_apcb
warn_no_apcb:
	printf "\n\t** WARNING **\n"
	printf "coreboot has been built without an APCB.\n"
	printf "This image will not boot.\n\n"

PHONY+=die_no_apcb
die_no_apcb: warn_no_apcb
	$(error This board requires the APCB to build correctly)

endif # ifeq ($(CONFIG_SOC_AMD_COMMON),y)
