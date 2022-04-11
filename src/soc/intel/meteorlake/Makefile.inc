ifeq ($(CONFIG_SOC_INTEL_METEORLAKE),y)

bootblock-y += bootblock/bootblock.c
bootblock-y += bootblock/ioe_die.c
bootblock-y += bootblock/report_platform.c
bootblock-y += bootblock/soc_die.c

CPPFLAGS_common += -I$(src)/soc/intel/meteorlake
CPPFLAGS_common += -I$(src)/soc/intel/meteorlake/include

endif
