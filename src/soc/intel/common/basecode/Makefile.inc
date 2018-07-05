ifeq ($(CONFIG_SOC_INTEL_COMMON_BASECODE),y)

subdirs-y += ./*

CPPFLAGS_common += -I$(src)/soc/intel/common/basecode/include/

endif
