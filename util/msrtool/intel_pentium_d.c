/*
 * This file is part of msrtool.
 *
 * Copyright (C) 2011 Anton Kochkov <anton.kochkov@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "msrtool.h"

int intel_pentium_d_probe(const struct targetdef *target, const struct cpuid_t *id) {
	return ((VENDOR_INTEL == id->vendor) &&
		(0xf == id->family) &&
		(0x6 == id->model));
}

const struct msrdef intel_pentium_d_msrs[] = {
	{0x0000, MSRTYPE_RDWR, MSR2(0, 0), "IA32_P5_MC_ADDR", "", {
		{ BITS_EOT }
	}},
	{0x0001, MSRTYPE_RDWR, MSR2(0, 0), "IA32_P5_MC_TYPE", "", {
		{ BITS_EOT }
	}},
	{0x0006, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MONITOR_FILTER_LINE_SIZE", "", {
		{ BITS_EOT }
	}},
	{0x0010, MSRTYPE_RDWR, MSR2(0, 0), "IA32_TIME_STAMP_COUNTER", "", {
		{ BITS_EOT }
	}},
	{0x0017, MSRTYPE_RDWR, MSR2(0, 0), "IA32_PLATFORM_ID", "", {
		{ BITS_EOT }
	}},
	{0x001B, MSRTYPE_RDWR, MSR2(0, 0), "IA32_APIC_BASE", "", {
		{ BITS_EOT }
	}},
	{0x002A, MSRTYPE_RDWR, MSR2(0, 0), "MSR_EBC_HARD_POWERON", "", {
		{ BITS_EOT }
	}},
	{0x002B, MSRTYPE_RDWR, MSR2(0, 0), "MSR_EBC_SOFT_POWERON", "", {
		{ BITS_EOT }
	}},
	{0x002C, MSRTYPE_RDWR, MSR2(0, 0), "MSR_EBC_FREQUENCY_ID", "", {
		{ BITS_EOT }
	}},
	{0x008B, MSRTYPE_RDWR, MSR2(0, 0), "IA32_BIOS_SIGN_ID", "", {
		{ BITS_EOT }
	}},
	{0x00FE, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRRCAP", "", {
		{ BITS_EOT }
	}},
	{0x0174, MSRTYPE_RDWR, MSR2(0, 0), "IA32_SYSENTER_CS", "", {
		{ BITS_EOT }
	}},
	{0x0175, MSRTYPE_RDWR, MSR2(0, 0), "IA32_SYSENTER_ESP", "", {
		{ BITS_EOT }
	}},
	{0x0176, MSRTYPE_RDWR, MSR2(0, 0), "IA32_SYSENTER_EIP", "", {
		{ BITS_EOT }
	}},
	{0x0179, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MCG_CAP", "", {
		{ BITS_EOT }
	}},
	{0x017A, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MCG_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x0180, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MCG_RAX", "", {
		{ BITS_EOT }
	}},
	{0x0181, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MCG_RBX", "", {
		{ BITS_EOT }
	}},
	{0x0182, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MCG_RCX", "", {
		{ BITS_EOT }
	}},
	{0x0183, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MCG_RDX", "", {
		{ BITS_EOT }
	}},
	{0x0184, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MCG_RSI", "", {
		{ BITS_EOT }
	}},
	{0x0185, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MCG_RDI", "", {
		{ BITS_EOT }
	}},
	{0x0186, MSRTYPE_RDWR, MSR2(0, 0), "IA32_PERF_EVNTSEL0", "", {
		{ BITS_EOT }
	}},
	{0x0187, MSRTYPE_RDWR, MSR2(0, 0), "IA32_PERF_EVNTSEL1", "", {
		{ BITS_EOT }
	}},
	{0x0188, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MCG_RFLAGS", "", {
		{ BITS_EOT }
	}},
	{0x0189, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MCG_RIP", "", {
		{ BITS_EOT }
	}},
	{0x018A, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MCG_MISC", "", {
		{ BITS_EOT }
	}},
	{0x0190, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MCG_R8", "", {
		{ BITS_EOT }
	}},
	{0x0191, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MCG_R9", "", {
		{ BITS_EOT }
	}},
	{0x0192, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MCG_R10", "", {
		{ BITS_EOT }
	}},
	{0x0193, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MCG_R11", "", {
		{ BITS_EOT }
	}},
	{0x0194, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MCG_R12", "", {
		{ BITS_EOT }
	}},
	{0x0195, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MCG_R13", "", {
		{ BITS_EOT }
	}},
	{0x0196, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MCG_R14", "", {
		{ BITS_EOT }
	}},
	{0x0197, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MCG_R15", "", {
		{ BITS_EOT }
	}},
	{0x0198, MSRTYPE_RDWR, MSR2(0, 0), "IA32_PERF_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x0199, MSRTYPE_RDWR, MSR2(0, 0), "IA32_PERF_CONTROL", "", {
		{ BITS_EOT }
	}},
	{0x019A, MSRTYPE_RDWR, MSR2(0, 0), "IA32_CLOCK_MODULATION", "", {
		{ BITS_EOT }
	}},
	{0x019B, MSRTYPE_RDWR, MSR2(0, 0), "IA32_THERM_INTERRUPT", "", {
		{ BITS_EOT }
	}},
	{0x019C, MSRTYPE_RDWR, MSR2(0, 0), "IA32_THERM_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x019D, MSRTYPE_RDWR, MSR2(0, 0), "GV_THERM", "", {
		{ BITS_EOT }
	}},
	{0x01A0, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MISC_ENABLES", "", {
		{ BITS_EOT }
	}},
	{0x01A1, MSRTYPE_RDWR, MSR2(0, 0), "MSR_PLATFORM_BRV", "", {
		{ BITS_EOT }
	}},
	{0x01A2, MSRTYPE_RDWR, MSR2(0, 0), "MSR_TEMPERATURE_TARGET", "", {
		{ BITS_EOT }
	}},
	{0x01D7, MSRTYPE_RDWR, MSR2(0, 0), "MSR_LER_FROM_LIP", "", {
		{ BITS_EOT }
	}},
	{0x01D8, MSRTYPE_RDWR, MSR2(0, 0), "MSR_LER_TO_LIP", "", {
		{ BITS_EOT }
	}},
	{0x01D9, MSRTYPE_RDWR, MSR2(0, 0), "IA32_DEBUGCTL", "", {
		{ BITS_EOT }
	}},
	{0x01DA, MSRTYPE_RDWR, MSR2(0, 0), "MSR_LASTBRANCH_TOS", "", {
		{ BITS_EOT }
	}},
	{0x0200, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_PHYSBASE0", "", {
		{ BITS_EOT }
	}},
	{0x0201, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_PHYSMASK0", "", {
		{ BITS_EOT }
	}},
	{0x0202, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_PHYSBASE1", "", {
		{ BITS_EOT }
	}},
	{0x0203, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_PHYSMASK1", "", {
		{ BITS_EOT }
	}},
	{0x0204, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_PHYSBASE2", "", {
		{ BITS_EOT }
	}},
	{0x0205, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_PHYSMASK2", "", {
		{ BITS_EOT }
	}},
	{0x0206, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_PHYSBASE3", "", {
		{ BITS_EOT }
	}},
	{0x0207, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_PHYSMASK3", "", {
		{ BITS_EOT }
	}},
	{0x0208, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_PHYSBASE4", "", {
		{ BITS_EOT }
	}},
	{0x0209, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_PHYSMASK4", "", {
		{ BITS_EOT }
	}},
	{0x020A, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_PHYSBASE5", "", {
		{ BITS_EOT }
	}},
	{0x020B, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_PHYSMASK5", "", {
		{ BITS_EOT }
	}},
	{0x020C, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_PHYSBASE6", "", {
		{ BITS_EOT }
	}},
	{0x020D, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_PHYSMASK6", "", {
		{ BITS_EOT }
	}},
	{0x020E, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_PHYSBASE7", "", {
		{ BITS_EOT }
	}},
	{0x020F, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_PHYSMASK7", "", {
		{ BITS_EOT }
	}},
	{0x0250, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_FIX64K_00000", "", {
		{ BITS_EOT }
	}},
	{0x0258, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_FIX16K_80000", "", {
		{ BITS_EOT }
	}},
	{0x0259, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_FIX16K_A0000", "", {
		{ BITS_EOT }
	}},
	{0x0268, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_FIX4K_C0000", "", {
		{ BITS_EOT }
	}},
	{0x0269, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_FIX4K_C8000", "", {
		{ BITS_EOT }
	}},
	{0x026A, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_FIX4K_D0000", "", {
		{ BITS_EOT }
	}},
	{0x026B, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_FIX4K_D8000", "", {
		{ BITS_EOT }
	}},
	{0x026C, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_FIX4K_E0000", "", {
		{ BITS_EOT }
	}},
	{0x026D, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_FIX4K_E8000", "", {
		{ BITS_EOT }
	}},
	{0x026E, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_FIX4K_F0000", "", {
		{ BITS_EOT }
	}},
	{0x026F, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_FIX4K_F8000", "", {
		{ BITS_EOT }
	}},
	{0x0277, MSRTYPE_RDWR, MSR2(0, 0), "IA32_PAT", "", {
		{ BITS_EOT }
	}},
	{0x02FF, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MTRR_DEF_TYPE", "", {
		{ BITS_EOT }
	}},
	{0x0300, MSRTYPE_RDWR, MSR2(0, 0), "MSR_BPU_COUNTER0", "", {
		{ BITS_EOT }
	}},
	{0x0301, MSRTYPE_RDWR, MSR2(0, 0), "MSR_BPU_COUNTER1", "", {
		{ BITS_EOT }
	}},
	{0x0302, MSRTYPE_RDWR, MSR2(0, 0), "MSR_BPU_COUNTER2", "", {
		{ BITS_EOT }
	}},
	{0x0303, MSRTYPE_RDWR, MSR2(0, 0), "MSR_BPU_COUNTER3", "", {
		{ BITS_EOT }
	}},
	{0x0304, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MS_COUNTER0", "", {
		{ BITS_EOT }
	}},
	{0x0305, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MS_COUNTER1", "", {
		{ BITS_EOT }
	}},
	{0x0306, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MS_COUNTER2", "", {
		{ BITS_EOT }
	}},
	{0x0307, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MS_COUNTER3", "", {
		{ BITS_EOT }
	}},
	{0x0308, MSRTYPE_RDWR, MSR2(0, 0), "MSR_FLAME_COUNTER0", "", {
		{ BITS_EOT }
	}},
	{0x0309, MSRTYPE_RDWR, MSR2(0, 0), "MSR_FLAME_COUNTER1", "", {
		{ BITS_EOT }
	}},
	{0x030A, MSRTYPE_RDWR, MSR2(0, 0), "IA32_FIXED_CTR1", "", {
		{ BITS_EOT }
	}},
	{0x030B, MSRTYPE_RDWR, MSR2(0, 0), "MSR_FLAME_COUNTER3", "", {
		{ BITS_EOT }
	}},
	{0x030C, MSRTYPE_RDWR, MSR2(0, 0), "MSR_IQ_COUNTER0", "", {
		{ BITS_EOT }
	}},
	{0x030D, MSRTYPE_RDWR, MSR2(0, 0), "MSR_IQ_COUNTER1", "", {
		{ BITS_EOT }
	}},
	{0x030E, MSRTYPE_RDWR, MSR2(0, 0), "MSR_IQ_COUNTER2", "", {
		{ BITS_EOT }
	}},
	{0x030F, MSRTYPE_RDWR, MSR2(0, 0), "MSR_IQ_COUNTER3", "", {
		{ BITS_EOT }
	}},
	{0x0310, MSRTYPE_RDWR, MSR2(0, 0), "MSR_IQ_COUNTER4", "", {
		{ BITS_EOT }
	}},
	{0x0311, MSRTYPE_RDWR, MSR2(0, 0), "MSR_IQ_COUNTER5", "", {
		{ BITS_EOT }
	}},
	{0x0345, MSRTYPE_RDWR, MSR2(0, 0), "IA32_PERF_CAPABILITIES", "", {
		{ BITS_EOT }
	}},
	{0x0360, MSRTYPE_RDWR, MSR2(0, 0), "MSR_BPU_CCCR0", "", {
		{ BITS_EOT }
	}},
	{0x0361, MSRTYPE_RDWR, MSR2(0, 0), "MSR_BPU_CCCR1", "", {
		{ BITS_EOT }
	}},
	{0x0362, MSRTYPE_RDWR, MSR2(0, 0), "MSR_BPU_CCCR2", "", {
		{ BITS_EOT }
	}},
	{0x0363, MSRTYPE_RDWR, MSR2(0, 0), "MSR_BPU_CCCR3", "", {
		{ BITS_EOT }
	}},
	{0x0364, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MS_CCCR0", "", {
		{ BITS_EOT }
	}},
	{0x0365, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MS_CCCR1", "", {
		{ BITS_EOT }
	}},
	{0x0366, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MS_CCCR2", "", {
		{ BITS_EOT }
	}},
	{0x0367, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MS_CCCR3", "", {
		{ BITS_EOT }
	}},
	{0x0368, MSRTYPE_RDWR, MSR2(0, 0), "MSR_FLAME_CCCR0", "", {
		{ BITS_EOT }
	}},
	{0x0369, MSRTYPE_RDWR, MSR2(0, 0), "MSR_FLAME_CCCR1", "", {
		{ BITS_EOT }
	}},
	{0x036A, MSRTYPE_RDWR, MSR2(0, 0), "MSR_FLAME_CCCR2", "", {
		{ BITS_EOT }
	}},
	{0x036B, MSRTYPE_RDWR, MSR2(0, 0), "MSR_FLAME_CCCR3", "", {
		{ BITS_EOT }
	}},
	{0x036C, MSRTYPE_RDWR, MSR2(0, 0), "MSR_IQ_CCCR0", "", {
		{ BITS_EOT }
	}},
	{0x036D, MSRTYPE_RDWR, MSR2(0, 0), "MSR_IQ_CCCR1", "", {
		{ BITS_EOT }
	}},
	{0x036E, MSRTYPE_RDWR, MSR2(0, 0), "MSR_IQ_CCCR2", "", {
		{ BITS_EOT }
	}},
	{0x036F, MSRTYPE_RDWR, MSR2(0, 0), "MSR_IQ_CCCR3", "", {
		{ BITS_EOT }
	}},
	{0x0370, MSRTYPE_RDWR, MSR2(0, 0), "MSR_IQ_CCCR4", "", {
		{ BITS_EOT }
	}},
	{0x0371, MSRTYPE_RDWR, MSR2(0, 0), "MSR_IQ_CCCR5", "", {
		{ BITS_EOT }
	}},
	{0x03A0, MSRTYPE_RDWR, MSR2(0, 0), "MSR_BSU_ESCR0", "", {
		{ BITS_EOT }
	}},
	{0x03A1, MSRTYPE_RDWR, MSR2(0, 0), "MSR_BSU_ESCR1", "", {
		{ BITS_EOT }
	}},
	{0x03A2, MSRTYPE_RDWR, MSR2(0, 0), "MSR_FSB_ESCR0", "", {
		{ BITS_EOT }
	}},
	{0x03A3, MSRTYPE_RDWR, MSR2(0, 0), "MSR_FSB_ESCR1", "", {
		{ BITS_EOT }
	}},
	{0x03A4, MSRTYPE_RDWR, MSR2(0, 0), "MSR_FIRM_ESCR0", "", {
		{ BITS_EOT }
	}},
	{0x03A5, MSRTYPE_RDWR, MSR2(0, 0), "MSR_FIRM_ESCR1", "", {
		{ BITS_EOT }
	}},
	{0x03A6, MSRTYPE_RDWR, MSR2(0, 0), "MSR_FLAME_ESCR0", "", {
		{ BITS_EOT }
	}},
	{0x03A7, MSRTYPE_RDWR, MSR2(0, 0), "MSR_FLAME_ESCR1", "", {
		{ BITS_EOT }
	}},
	{0x03A8, MSRTYPE_RDWR, MSR2(0, 0), "MSR_DAC_ESCR0", "", {
		{ BITS_EOT }
	}},
	{0x03A9, MSRTYPE_RDWR, MSR2(0, 0), "MSR_DAC_ESCR1", "", {
		{ BITS_EOT }
	}},
	{0x03AA, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MOB_ESCR0", "", {
		{ BITS_EOT }
	}},
	{0x03AB, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MOB_ESCR1", "", {
		{ BITS_EOT }
	}},
	{0x03AC, MSRTYPE_RDWR, MSR2(0, 0), "MSR_PMH_ESCR0", "", {
		{ BITS_EOT }
	}},
	{0x03AD, MSRTYPE_RDWR, MSR2(0, 0), "MSR_PMH_ESCR1", "", {
		{ BITS_EOT }
	}},
	{0x03AE, MSRTYPE_RDWR, MSR2(0, 0), "MSR_SAAT_ESCR0", "", {
		{ BITS_EOT }
	}},
	{0x03AF, MSRTYPE_RDWR, MSR2(0, 0), "MSR_SAAT_ESCR1", "", {
		{ BITS_EOT }
	}},
	{0x03B0, MSRTYPE_RDWR, MSR2(0, 0), "MSR_U2L_ESCR0", "", {
		{ BITS_EOT }
	}},
	{0x03B1, MSRTYPE_RDWR, MSR2(0, 0), "MSR_U2L_ESCR1", "", {
		{ BITS_EOT }
	}},
	{0x03B2, MSRTYPE_RDWR, MSR2(0, 0), "MSR_BPU_ESCR0", "", {
		{ BITS_EOT }
	}},
	{0x03B3, MSRTYPE_RDWR, MSR2(0, 0), "MSR_BPU_ESCR1", "", {
		{ BITS_EOT }
	}},
	{0x03B4, MSRTYPE_RDWR, MSR2(0, 0), "MSR_IS_ESCR0", "", {
		{ BITS_EOT }
	}},
	{0x03B5, MSRTYPE_RDWR, MSR2(0, 0), "MSR_BPU_ESCR1", "", {
		{ BITS_EOT }
	}},
	{0x03B6, MSRTYPE_RDWR, MSR2(0, 0), "MSR_ITLB_ESCR0", "", {
		{ BITS_EOT }
	}},
	{0x03B7, MSRTYPE_RDWR, MSR2(0, 0), "MSR_ITLB_ESCR1", "", {
		{ BITS_EOT }
	}},
	{0x03B8, MSRTYPE_RDWR, MSR2(0, 0), "MSR_CRU_ESCR0", "", {
		{ BITS_EOT }
	}},
	{0x03B9, MSRTYPE_RDWR, MSR2(0, 0), "MSR_CRU_ESCR1", "", {
		{ BITS_EOT }
	}},
	{0x03BA, MSRTYPE_RDWR, MSR2(0, 0), "MSR_IQ_ESCR0", "", {
		{ BITS_EOT }
	}},
	{0x03BB, MSRTYPE_RDWR, MSR2(0, 0), "MSR_IQ_ESCR1", "", {
		{ BITS_EOT }
	}},
	{0x03BC, MSRTYPE_RDWR, MSR2(0, 0), "MSR_RAT_ESCR0", "", {
		{ BITS_EOT }
	}},
	{0x03BD, MSRTYPE_RDWR, MSR2(0, 0), "MSR_RAT_ESCR1", "", {
		{ BITS_EOT }
	}},
	{0x03BE, MSRTYPE_RDWR, MSR2(0, 0), "MSR_SSU_ESCR0", "", {
		{ BITS_EOT }
	}},
	{0x03C0, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MS_ESCR0", "", {
		{ BITS_EOT }
	}},
	{0x03C1, MSRTYPE_RDWR, MSR2(0, 0), "MSR_MS_ESCR1", "", {
		{ BITS_EOT }
	}},
	{0x03C2, MSRTYPE_RDWR, MSR2(0, 0), "MSR_TBPU_ESCR0", "", {
		{ BITS_EOT }
	}},
	{0x03C3, MSRTYPE_RDWR, MSR2(0, 0), "MSR_TBPU_ESCR1", "", {
		{ BITS_EOT }
	}},
	{0x03C4, MSRTYPE_RDWR, MSR2(0, 0), "MSR_TC_ESCR0", "", {
		{ BITS_EOT }
	}},
	{0x03C5, MSRTYPE_RDWR, MSR2(0, 0), "MSR_TC_ESCR1", "", {
		{ BITS_EOT }
	}},
	{0x03C8, MSRTYPE_RDWR, MSR2(0, 0), "MSR_IX_ESCR0", "", {
		{ BITS_EOT }
	}},
	{0x03C9, MSRTYPE_RDWR, MSR2(0, 0), "MSR_IX_ESCR1", "", {
		{ BITS_EOT }
	}},
	{0x03CA, MSRTYPE_RDWR, MSR2(0, 0), "MSR_ALF_ESCR0", "", {
		{ BITS_EOT }
	}},
	{0x03CB, MSRTYPE_RDWR, MSR2(0, 0), "MSR_ALF_ESCR1", "", {
		{ BITS_EOT }
	}},
	{0x03CC, MSRTYPE_RDWR, MSR2(0, 0), "MSR_CRU_ESCR2", "", {
		{ BITS_EOT }
	}},
	{0x03CD, MSRTYPE_RDWR, MSR2(0, 0), "MSR_CRU_ESCR3", "", {
		{ BITS_EOT }
	}},
	{0x03E0, MSRTYPE_RDWR, MSR2(0, 0), "MSR_CRU_ESCR4", "", {
		{ BITS_EOT }
	}},
	{0x03E1, MSRTYPE_RDWR, MSR2(0, 0), "MSR_CRU_ESCR5", "", {
		{ BITS_EOT }
	}},
	{0x03F0, MSRTYPE_RDWR, MSR2(0, 0), "MSR_TC_PRECISE_EVENT", "", {
		{ BITS_EOT }
	}},
	{0x03F1, MSRTYPE_RDWR, MSR2(0, 0), "MSR_PEBS_ENABLE", "", {
		{ BITS_EOT }
	}},
	{0x03F2, MSRTYPE_RDWR, MSR2(0, 0), "MSR_PEBS_MATRIX_VERT", "", {
		{ BITS_EOT }
	}},
	{0x0400, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MC0_CTL", "", {
		{ BITS_EOT }
	}},
	{0x0401, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MC0_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x0402, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MC0_ADDR", "", {
		{ BITS_EOT }
	}},
	{0x0403, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MC0_MISC", "", {
		{ BITS_EOT }
	}},
	{0x0404, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MC1_CTL", "", {
		{ BITS_EOT }
	}},
	{0x0405, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MC1_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x0406, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MC1_ADDR", "", {
		{ BITS_EOT }
	}},
	{0x0408, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MC2_CTL", "", {
		{ BITS_EOT }
	}},
	{0x0409, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MC2_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x040C, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MC3_CTL", "", {
		{ BITS_EOT }
	}},
	{0x040D, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MC3_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x040E, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MC3_ADDR", "", {
		{ BITS_EOT }
	}},
	{0x040F, MSRTYPE_RDWR, MSR2(0, 0), "IA32_MC3_MISC", "", {
		{ BITS_EOT }
	}},
	{0x0600, MSRTYPE_RDWR, MSR2(0, 0), "IA32_DS_AREA", "", {
		{ BITS_EOT }
	}},
	{ MSR_EOT }
};
