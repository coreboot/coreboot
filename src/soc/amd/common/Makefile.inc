ifeq ($(CONFIG_SOC_AMD_COMMON),y)
subdirs-y += block
subdirs-y += fsp
subdirs-y += vboot
endif
