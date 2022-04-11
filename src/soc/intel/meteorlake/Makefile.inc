ifeq ($(CONFIG_SOC_INTEL_METEORLAKE),y)

subdirs-y += romstage
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo

bootblock-y += bootblock/bootblock.c
bootblock-y += bootblock/ioe_die.c
bootblock-y += bootblock/report_platform.c
bootblock-y += bootblock/soc_die.c
bootblock-y += espi.c
bootblock-y += p2sb.c

romstage-y += espi.c
romstage-y += meminit.c
romstage-y += reset.c

CPPFLAGS_common += -I$(src)/soc/intel/meteorlake
CPPFLAGS_common += -I$(src)/soc/intel/meteorlake/include

endif
