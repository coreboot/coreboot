ifeq ($(CONFIG_SOC_AMD_COMMON),y)

CPPFLAGS_common += -I$(src)/soc/amd/common

subdirs-$(CONFIG_SOC_AMD_COMMON_BLOCK) += block

endif
