ifeq ($(CONFIG_SOC_INTEL_ALDERLAKE),y)
subdirs-y += romstage
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/smm
subdirs-y += ../../../cpu/x86/tsc

bootblock-y += bootblock/bootblock.c
bootblock-y += bootblock/cpu.c
bootblock-y += bootblock/pch.c
bootblock-y += bootblock/report_platform.c
bootblock-y += espi.c
bootblock-y += p2sb.c

romstage-y += espi.c
romstage-y += meminit.c
romstage-y += reset.c
CPPFLAGS_common += -I$(src)/soc/intel/alderlake
CPPFLAGS_common += -I$(src)/soc/intel/alderlake/include
endif
