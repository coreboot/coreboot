## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOC_INTEL_GRANITERAPIDS),y)

subdirs-y += ../../../../cpu/intel/turbo
subdirs-y += ../../../../cpu/x86/lapic
subdirs-y += ../../../../cpu/x86/mtrr
subdirs-y += ../../../../cpu/x86/smm
subdirs-y += ../../../../cpu/x86/tsc
subdirs-y += ../../../../cpu/intel/microcode

romstage-y += romstage.c
romstage-y += soc_util.c
romstage-y += soc_iio.c
romstage-$(CONFIG_DISPLAY_UPD_DATA) += upd_display.c

ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-y += soc_util.c
ramstage-y += ramstage.c
ramstage-y += soc_acpi.c
ramstage-y += ../chip_gen6.c

CPPFLAGS_common += -I$(src)/soc/intel/xeon_sp/gnr/include
CPPFLAGS_common += -I$(src)/soc/intel/xeon_sp/gnr

CFLAGS_common += -fshort-wchar

endif ## CONFIG_SOC_INTEL_GRANITERAPIDS
