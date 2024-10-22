## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOC_INTEL_SAPPHIRERAPIDS_SP),y)

subdirs-y += ../../../../cpu/intel/turbo
subdirs-y += ../../../../cpu/x86/lapic
subdirs-y += ../../../../cpu/x86/mtrr
subdirs-y += ../../../../cpu/x86/tsc
subdirs-y += ../../../../cpu/intel/microcode

romstage-y += romstage.c soc_util.c
romstage-y += ../dimm.c
romstage-$(CONFIG_DISPLAY_HOBS) += hob_display.c
romstage-$(CONFIG_DISPLAY_UPD_DATA) += upd_display.c

ramstage-y += chip.c cpu.c soc_util.c ramstage.c soc_acpi.c reset.c
ramstage-y += crashlog.c ioat.c
ramstage-y += ../chip_gen1.c ../lpc_gen1.c
ramstage-$(CONFIG_DISPLAY_HOBS) += hob_display.c
ramstage-$(CONFIG_DISPLAY_UPD_DATA) += upd_display.c
CPPFLAGS_common += -I$(src)/soc/intel/xeon_sp/spr/include -I$(src)/soc/intel/xeon_sp/spr

cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-8f-08
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-cf-02

endif ## CONFIG_SOC_INTEL_SAPPHIRERAPIDS_SP
