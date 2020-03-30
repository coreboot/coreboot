##
## SPDX-License-Identifier: GPL-2.0-only
## This file is part of the coreboot project.
##

ifeq ($(CONFIG_SOC_INTEL_COOPERLAKE_SP),y)

subdirs-y += ../../../../cpu/x86/lapic
subdirs-y += ../../../../cpu/x86/mtrr
subdirs-y += ../../../../cpu/x86/tsc
subdirs-y += ../../../../cpu/intel/microcode

romstage-y += romstage.c
ramstage-y += chip.c acpi.c cpu.c

CPPFLAGS_common += -I$(src)/soc/intel/xeon_sp/cpx/include -I$(src)/soc/intel/xeon_sp/cpx

endif ## CONFIG_SOC_INTEL_COOPERLAKE_SP
