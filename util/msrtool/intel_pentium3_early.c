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

int intel_pentium3_early_probe(const struct targetdef *target, const struct cpuid_t *id) {
	return ((VENDOR_INTEL == id->vendor) &&
		(0x6 == id->family) && (
		(0x7 == id->model) ||
		(0x8 == id->model)
		));
}

const struct msrdef intel_pentium3_early_msrs[] = {
	{0x0, MSRTYPE_RDWR, MSR2(0,0), "IA32_P5_MC_ADDR", "", {
		{ BITS_EOT }
	}},
	{0x1, MSRTYPE_RDWR, MSR2(0,0), "IA32_P5_MC_TYPE", "", {
		{ BITS_EOT }
	}},
	{0x10, MSRTYPE_RDWR, MSR2(0,0), "IA32_TIME_STAMP_COUNTER", "", {
		{ BITS_EOT }
	}},
	{0x17, MSRTYPE_RDWR, MSR2(0,0), "IA32_PLATFORM_ID", "", {
		{ BITS_EOT }
	}},
	{0x1b, MSRTYPE_RDWR, MSR2(0,0), "IA32_APIC_BASE", "", {
		{ BITS_EOT }
	}},
	{0x2a, MSRTYPE_RDWR, MSR2(0,0), "EBL_CR_POWERON", "", {
		{ BITS_EOT }
	}},
	{0x33, MSRTYPE_RDWR, MSR2(0,0), "TEST_CTL", "", {
		{ BITS_EOT }
	}},
	{0x88, MSRTYPE_RDWR, MSR2(0,0), "BBL_CR_D0", "", {
		{ BITS_EOT }
	}},
	{0x89, MSRTYPE_RDWR, MSR2(0,0), "BBL_CR_D1", "", {
		{ BITS_EOT }
	}},
	{0x8a, MSRTYPE_RDWR, MSR2(0,0), "BBL_CR_D2", "", {
		{ BITS_EOT }
	}},
	{0x8b, MSRTYPE_RDWR, MSR2(0,0), "IA32_BIOS_SIGN_ID", "", {
		{ BITS_EOT }
	}},
	{0xc1, MSRTYPE_RDWR, MSR2(0,0), "PERFCTR0", "", {
		{ BITS_EOT }
	}},
	{0xc2, MSRTYPE_RDWR, MSR2(0,0), "PERFCTR1", "", {
		{ BITS_EOT }
	}},
	{0xfe, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRRCAP", "", {
		{ BITS_EOT }
	}},
	{0x116, MSRTYPE_RDWR, MSR2(0,0), "BBL_CR_ADDR", "", {
		{ BITS_EOT }
	}},
	{0x118, MSRTYPE_RDWR, MSR2(0,0), "BBL_CR_DECC", "", {
		{ BITS_EOT }
	}},
	{0x119, MSRTYPE_RDWR, MSR2(0,0), "BBL_CR_CTL", "", {
		{ BITS_EOT }
	}},
	{0x11b, MSRTYPE_RDWR, MSR2(0,0), "BBL_CR_BUSY", "", {
		{ BITS_EOT }
	}},
	{0x11e, MSRTYPE_RDWR, MSR2(0,0), "BBL_CR_CTL3", "", {
		{ BITS_EOT }
	}},
	{0x174, MSRTYPE_RDWR, MSR2(0,0), "IA32_SYSENTER_CS", "", {
		{ BITS_EOT }
	}},
	{0x175, MSRTYPE_RDWR, MSR2(0,0), "IA32_SYSENTER_ESP", "", {
		{ BITS_EOT }
	}},
	{0x176, MSRTYPE_RDWR, MSR2(0,0), "IA32_SYSENTER_EIP", "", {
		{ BITS_EOT }
	}},
	{0x179, MSRTYPE_RDWR, MSR2(0,0), "IA32_MCG_CAP", "", {
		{ BITS_EOT }
	}},
	{0x17a, MSRTYPE_RDWR, MSR2(0,0), "IA32_MCG_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x17b, MSRTYPE_RDWR, MSR2(0,0), "IA32_MCG_CTL", "", {
		{ BITS_EOT }
	}},
	{0x186, MSRTYPE_RDWR, MSR2(0,0), "IA32_PERF_EVNTSEL0", "", {
		{ BITS_EOT }
	}},
	{0x187, MSRTYPE_RDWR, MSR2(0,0), "IA32_PERF_EVNTSEL1", "", {
		{ BITS_EOT }
	}},
	{0x1d9, MSRTYPE_RDWR, MSR2(0,0), "IA32_DEBUGCTL", "", {
		{ BITS_EOT }
	}},
	{0x1db, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCHFROMIP", "", {
		{ BITS_EOT }
	}},
	{0x1dc, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCHTOIP", "", {
		{ BITS_EOT }
	}},
	{0x1dd, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTINTFROMIP", "", {
		{ BITS_EOT }
	}},
	{0x1de, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTINTTOIP", "", {
		{ BITS_EOT }
	}},
	{0x1e0, MSRTYPE_RDWR, MSR2(0,0), "MSR_ROB_CR_BKUPTMPDR6", "", {
		{ BITS_EOT }
	}},
	{0x200, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYSBASE0", "", {
		{ BITS_EOT }
	}},
	{0x201, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYSMASK0", "", {
		{ BITS_EOT }
	}},
	{0x202, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYSBASE1", "", {
		{ BITS_EOT }
	}},
	{0x203, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYSMASK1", "", {
		{ BITS_EOT }
	}},
	{0x204, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYSBASE2", "", {
		{ BITS_EOT }
	}},
	{0x205, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYSMASK2", "", {
		{ BITS_EOT }
	}},
	{0x206, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYSBASE3", "", {
		{ BITS_EOT }
	}},
	{0x207, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYSMASK3", "", {
		{ BITS_EOT }
	}},
	{0x208, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYSBASE4", "", {
		{ BITS_EOT }
	}},
	{0x209, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYSMASK4", "", {
		{ BITS_EOT }
	}},
	{0x20a, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYSBASE5", "", {
		{ BITS_EOT }
	}},
	{0x20b, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYSMASK5", "", {
		{ BITS_EOT }
	}},
	{0x20c, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYSBASE6", "", {
		{ BITS_EOT }
	}},
	{0x20d, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYSMASK6", "", {
		{ BITS_EOT }
	}},
	{0x20e, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYSBASE7", "", {
		{ BITS_EOT }
	}},
	{0x20f, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYSMASK7", "", {
		{ BITS_EOT }
	}},
	{0x250, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_FIX64K_00000", "", {
		{ BITS_EOT }
	}},
	{0x258, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_FIX16K_80000", "", {
		{ BITS_EOT }
	}},
	{0x259, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_FIX16K_A0000", "", {
		{ BITS_EOT }
	}},
	{0x268, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_FIX4K_C0000", "", {
		{ BITS_EOT }
	}},
	{0x269, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_FIX4K_C8000", "", {
		{ BITS_EOT }
	}},
	{0x26a, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_FIX4K_D0000", "", {
		{ BITS_EOT }
	}},
	{0x26b, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_FIX4K_D8000", "", {
		{ BITS_EOT }
	}},
	{0x26c, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_FIX4K_E0000", "", {
		{ BITS_EOT }
	}},
	{0x26d, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_FIX4K_E8000", "", {
		{ BITS_EOT }
	}},
	{0x26e, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_FIX4K_F0000", "", {
		{ BITS_EOT }
	}},
	{0x26f, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_FIX4K_F8000", "", {
		{ BITS_EOT }
	}},
	{0x2ff, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_DEF_TYPE", "", {
		{ BITS_EOT }
	}},
	{0x400, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC0_CTL", "", {
		{ BITS_EOT }
	}},
	{0x401, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC0_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x402, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC0_ADDR", "", {
		{ BITS_EOT }
	}},
	{0x404, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC1_CTL", "", {
		{ BITS_EOT }
	}},
	{0x405, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC1_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x406, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC1_ADDR", "", {
		{ BITS_EOT }
	}},
	{0x408, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC2_CTL", "", {
		{ BITS_EOT }
	}},
	{0x409, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC2_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x40a, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC2_ADDR", "", {
		{ BITS_EOT }
	}},
	{0x40c, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC3_CTL", "", {
		{ BITS_EOT }
	}},
	{0x40d, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC3_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x40e, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC3_ADDR", "", {
		{ BITS_EOT }
	}},
	{0x410, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC4_CTL", "", {
		{ BITS_EOT }
	}},
	{0x411, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC4_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x412, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC4_ADDR", "", {
		{ BITS_EOT }
	}},
	{ MSR_EOT }
};
