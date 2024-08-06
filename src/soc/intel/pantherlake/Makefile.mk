## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOC_INTEL_PANTHERLAKE_BASE),y)

subdirs-y += romstage
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo

bootblock-y += bootblock/bootblock.c
bootblock-y += bootblock/pcd.c
bootblock-y += bootblock/report_platform.c
bootblock-y += espi.c
bootblock-y += gpio.c
bootblock-y += soc_info.c

romstage-y += espi.c
romstage-y += gpio.c
romstage-y += meminit.c
romstage-y += pcie_rp.c
romstage-y += reset.c
romstage-y += soc_info.c

CPPFLAGS_common += -I$(src)/soc/intel/pantherlake
CPPFLAGS_common += -I$(src)/soc/intel/pantherlake/include

endif
