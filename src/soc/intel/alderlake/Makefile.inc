ifeq ($(CONFIG_SOC_INTEL_ALDERLAKE),y)
bootblock-y += bootblock/bootblock.c
bootblock-y += bootblock/cpu.c
bootblock-y += bootblock/pch.c
bootblock-y += bootblock/report_platform.c
CPPFLAGS_common += -I$(src)/soc/intel/alderlake
CPPFLAGS_common += -I$(src)/soc/intel/alderlake/include
endif
