ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK),y)

subdirs-y += ./*

CPPFLAGS_common += -I$(src)/soc/amd/common/block/include/

endif
