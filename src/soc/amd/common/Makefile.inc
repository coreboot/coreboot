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

endif
