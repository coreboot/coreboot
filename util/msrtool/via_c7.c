/*
 * This file is part of msrtool.
 *
 * Copyright (C) 2011 Anton Kochkov <anton.kochkov@gmail.com>
 * Copyright (C) 2017 Lubomir Rintel <lkundrak@v3.sk>
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

int via_c7_probe(const struct targetdef *target, const struct cpuid_t *id) {
	return ((VENDOR_CENTAUR == id->vendor) &&
		(0x6 == id->family) && (
		(0xa == id->model) || /* C7 A */
		(0xd == id->model) || /* C7 D */
		(0xf == id->model)    /* Nano */
		));
}

const struct msrdef via_c7_msrs[] = {
	{0x10, MSRTYPE_RDWR, MSR2(0,0), "IA32_TIME_STAMP_COUNTER", "", {
		{ BITS_EOT }
	}},
	{0x2a, MSRTYPE_RDWR, MSR2(0,0), "EBL_CR_POWERON", "", {
		{ BITS_EOT }
	}},
	{0xc1, MSRTYPE_RDWR, MSR2(0,0), "PERFCTR0", "", {
		{ BITS_EOT }
	}},
	{0xc2, MSRTYPE_RDWR, MSR2(0,0), "PERFCTR1", "", {
		{ BITS_EOT }
	}},
	{0x11e, MSRTYPE_RDWR, MSR2(0,0), "BBL_CR_CTL3", "", {
		{ BITS_EOT }
	}},
	/* if CPUID.0AH: EAX[15:8] > 0 */
	{0x186, MSRTYPE_RDWR, MSR2(0,0), "IA32_PERFEVTSEL0",
			"Performance Event Select Register 0", {
		{ 63, 32, RESERVED },
		{ 31, 8, "CMASK", "R/W", PRESENT_HEX, {
			/* When CMASK is not zero, the corresponding performance
			 * counter 0 increments each cycle if the event count
			 * is greater than or equal to the CMASK.
			 */
			{ BITVAL_EOT }
		}},
		{ 23, 1, "INV", "R/W", PRESENT_BIN, {
			{ MSR1(0), "CMASK using as is" },
			{ MSR1(1), "CMASK inerting" },
			{ BITVAL_EOT }
		}},
		{ 22, 1, "EN", "R/W", PRESENT_BIN, {
			{ MSR1(0), "No commence counting" },
			{ MSR1(1), "Commence counting" },
			{ BITVAL_EOT }
		}},
		{ 21, 1, "AnyThread", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 20, 1, "INT", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Interrupt on counter overflow is disabled" },
			{ MSR1(1), "Interrupt on counter overflow is enabled" },
			{ BITVAL_EOT }
		}},
		{ 19, 1, "PC", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Disabled pin control" },
			{ MSR1(1), "Enabled pin control" },
			{ BITVAL_EOT }
		}},
		{ 18, 1, "Edge", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Disabled edge detection" },
			{ MSR1(1), "Enabled edge detection" },
			{ BITVAL_EOT }
		}},
		{ 17, 1, "OS", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "Counts while in privilege level is ring 0" },
			{ BITVAL_EOT }
		}},
		{ 16, 1, "USR", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "Counts while in privilege level is not ring 0" },
			{ BITVAL_EOT }
		}},
		{ 15, 8, "UMask", "R/W", PRESENT_HEX, {
			/* Qualifies the microarchitectural condition
			 * to detect on the selected event logic. */
			{ BITVAL_EOT }
		}},
		{ 7, 8, "Event Select", "R/W", PRESENT_HEX, {
			/* Selects a performance event logic unit. */
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	/* if CPUID.0AH: EAX[15:8] > 0 */
	{0x187, MSRTYPE_RDWR, MSR2(0,0), "IA32_PERFEVTSEL1",
			"Performance Event Select Register 1", {
		{ 63, 32, RESERVED },
		{ 31, 8, "CMASK", "R/W", PRESENT_HEX, {
			/* When CMASK is not zero, the corresponding performance
			 * counter 1 increments each cycle if the event count
			 * is greater than or equal to the CMASK.
			 */
			{ BITVAL_EOT }
		}},
		{ 23, 1, "INV", "R/W", PRESENT_BIN, {
			{ MSR1(0), "CMASK using as is" },
			{ MSR1(1), "CMASK inerting" },
			{ BITVAL_EOT }
		}},
		{ 22, 1, "EN", "R/W", PRESENT_BIN, {
			{ MSR1(0), "No commence counting" },
			{ MSR1(1), "Commence counting" },
			{ BITVAL_EOT }
		}},
		{ 21, 1, "AnyThread", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 20, 1, "INT", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Interrupt on counter overflow is disabled" },
			{ MSR1(1), "Interrupt on counter overflow is enabled" },
			{ BITVAL_EOT }
		}},
		{ 19, 1, "PC", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Disabled pin control" },
			{ MSR1(1), "Enabled pin control" },
			{ BITVAL_EOT }
		}},
		{ 18, 1, "Edge", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Disabled edge detection" },
			{ MSR1(1), "Enabled edge detection" },
			{ BITVAL_EOT }
		}},
		{ 17, 1, "OS", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "Counts while in privilege level is ring 0" },
			{ BITVAL_EOT }
		}},
		{ 16, 1, "USR", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "Counts while in privilege level is not ring 0" },
			{ BITVAL_EOT }
		}},
		{ 15, 8, "UMask", "R/W", PRESENT_HEX, {
			/* Qualifies the microarchitectural condition
			 * to detect on the selected event logic. */
			{ BITVAL_EOT }
		}},
		{ 7, 8, "Event Select", "R/W", PRESENT_HEX, {
			/* Selects a performance event logic unit. */
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x198, MSRTYPE_RDWR, MSR2(0,0), "IA32_PERF_STATUS", "", {
		{ 63, 8, "Lowest Supported Clock Ratio", "R/O", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 55, 8, "Lowest Supported Voltage", "R/O", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 47, 8, "Highest Supported Clock Ratio", "R/O", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 39, 8, "Highest Supported Voltage", "R/O", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 31, 8, "Lowest Clock Ratio", "R/O", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 23, 2, RESERVED },
		{ 21, 2, "Performance Control MSR Transition", "R/O", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 1, "Thermal Monitor 2 transition", "R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 18, 1, "Thermal Monitor 2 transition", "R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 17, 1, "Voltage Transition in progress", "R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 16, 1, "Clock Ratio Transition in progress", "R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 15, 8, "Current Clock Ratio", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 7, 8, "16*x + 700 = Current voltage in mV", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x199, MSRTYPE_RDWR, MSR2(0,0), "IA32_PERF_CTL", "", {
		{ 63, 48, RESERVED },
		{ 15, 8, "Desired Clock Ratio", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 7, 8, "16*x + 700 = Desired voltage in mV", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x19a, MSRTYPE_RDWR, MSR2(0,0), "IA32_CLOCK_MODULATION", "", {
		{ 63, 59, RESERVED },
		{ 15, 8, "allows selection of the on-demand clock modulation duty cycle", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Reserved" },
			{ MSR1(1), "12.5%" },
			{ MSR1(2), "25.0%" },
			{ MSR1(3), "37.5%" },
			{ MSR1(4), "50.0%" },
			{ MSR1(5), "62.5%" },
			{ MSR1(6), "75.0%" },
			{ MSR1(7), "87.5%" },
			{ BITVAL_EOT }
		}},
		{ 0, 1, RESERVED },
		{ BITS_EOT }
	}},
	{0x19b, MSRTYPE_RDWR, MSR2(0,0), "IA32_THERM_INTERRUPT", "", {
		{ 63, 62, RESERVED },
		{ 1, 1, "Enables APIC LVT interrupt on a low-to-high temp transition", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 0, 1, "Enables APIC LVT interrupt on a high-to-low temp transition", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x19c, MSRTYPE_RDWR, MSR2(0,0), "IA32_THERM_STATUS", "", {
		{ 63, 62, RESERVED },
		{ 1, 1, "TCC assert detect", "R/O", PRESENT_BIN, {
			{ MSR1(0), "TCC not asserted" },
			{ MSR1(1), "TCC asserted" },
			{ BITVAL_EOT }
		}},
		{ 0, 1, "TCC trigger detect (Sticky bit, only cleared upon reset)", "R/O", PRESENT_BIN, {
			{ MSR1(0), "TCC not triggered" },
			{ MSR1(1), "TCC triggered" },
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x19d, MSRTYPE_RDWR, MSR2(0,0), "MSR_THERM2_CTL", "", {
		{ 63, 47, RESERVED },
		{ 16, 1, "Thermal Monitor enable", "R/W", PRESENT_HEX, {
			{ MSR1(0), "Thermal Monitor 1 enabled" },
			{ MSR1(1), "Thermal Monitor 2 enabled" },
			{ BITVAL_EOT }
		}},
		{ 15, 8, "Thermal Monitor 2 performance state clock ratio", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 7, 8, "Thermal Monitor 2 performance state volatege", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x1a0, MSRTYPE_RDWR, MSR2(0,0), "IA32_MISC_ENABLES", "", {
		{ 63, 43, RESERVED },
		{ 20, 1, "PowerSaver lock", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Bit 16 can be set and cleared." },
			{ MSR1(1), "Bit 16 can only be cleared upon reset." },
			{ BITVAL_EOT }
		}},
		{ 19, 3, RESERVED },
		{ 16, 1, "Enhanced PowerSaver enable", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Performance state changes disabled" },
			{ MSR1(1), "Performance state changes enabled" },
			{ BITVAL_EOT }
		}},
		{ 15, 5, RESERVED },
		{ 10, 1, "PBE enable", "R/W", PRESENT_BIN, {
			{ MSR1(0), "FERR# legacy mode" },
			{ MSR1(1), "Enables break events for APIC via FERR#" },
			{ BITVAL_EOT }
		}},
		{ 9, 6, RESERVED },
		{ 3, 1, "Thermal Monitor 2 enable", "R/W", PRESENT_BIN, {
			{ MSR1(0), "On-die clock throttling enabled" },
			{ MSR1(1), "Thermal Monitor 1 or 2 enabled" },
			{ BITVAL_EOT }
		}},
		{ 2, 3, RESERVED },
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
	{0x1107, MSRTYPE_RDWR, MSR2(0,0), "FCR",
			"Feature Control Register", {
		{ 63, 55, RESERVED },
		{ 8, 1, "Disables L2 Cache", "R/W", PRESENT_BIN, {
			{ MSR1(0), "L2 Cache enabled" },
			{ MSR1(1), "L2 Cache disabled" },
			{ BITVAL_EOT }
		}},
		{ 7, 6, RESERVED },
		{ 1, 1, "Enables CPUID reporting CMPXCHG8B", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Disabled CPUID reporting CMPXCHG8B" },
			{ MSR1(1), "Enabled CPUID reporting CMPXCHG8B" },
			{ BITVAL_EOT }
		}},
		{ 0, 1, RESERVED },
		{ BITS_EOT }
	}},
	{0x1108, MSRTYPE_RDWR, MSR2(0,0), "FCR2",
			"Feature Control Register 2", {
		{ 63, 32, "Last 4 characters of Alternate Vendor ID string", "R/W", PRESENT_STR, {
			{ BITVAL_EOT }
		}},
		{ 31, 17, RESERVED },
		{ 14, 1, "Use the Alternate Vendor ID string", "R/W", PRESENT_BIN, {
			{ MSR1(0), "The CPUID instruction vendor ID is CentaurHauls" },
			{ MSR1(1), "The CPUID instruction returns the alternate Vendor ID" },
			{ BITVAL_EOT }
		}},
		{ 13, 2, RESERVED },
		{ 11, 4, "Family ID", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 7, 4, "Model ID", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 3, 4, RESERVED },
		{ BITS_EOT }
	}},
	{0x1109, MSRTYPE_WRONLY, MSR2(0,0), "FCR3",
			"Feature Control Register 3", {
		{ 63, 32, "First 4 characters of Alternate Vendor ID string", "W/O", PRESENT_STR, {
			{ BITVAL_EOT }
		}},
		{ 31, 32, "Middle 4 characters of Alternate Vendor ID string", "W/O", PRESENT_STR, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x1152, MSRTYPE_RDONLY, MSR2(0,0), "FUSES", "Fuses", {
		{ BITS_EOT }
	}},
	{0x1153, MSRTYPE_RDONLY, MSR2(0,0), "BRAND",
			"BRAND_1 XOR BRAND_2, (00b = C7-M, 01b = C7, 10b = Eden, 11b = Reserved)", {
		{ 63, 42, RESERVED },
		{ 21, 2, "BRAND_1", "R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 19, 2, "BRAND_2", "R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 17, 18, RESERVED },
		{ BITS_EOT }
	}},
	{0x1160, MSRTYPE_RDWR, MSR2(0,0), "UNK0", "", {
		{ BITS_EOT }
	}},
	{0x1161, MSRTYPE_RDWR, MSR2(0,0), "UNK1", "", {
		{ BITS_EOT }
	}},
	{0x1164, MSRTYPE_RDWR, MSR2(0,0), "THERM_THRESH_LOW", "(FUSES[6:4] * 5 + 65)", {
		{ BITS_EOT }
	}},
	{0x1165, MSRTYPE_RDWR, MSR2(0,0), "THERM_THRESH_HI", "(FUSES[6:4] * 5 + 65) + 5", {
		{ BITS_EOT }
	}},
	{0x1166, MSRTYPE_RDWR, MSR2(0,0), "THERM_THRESH_OVERSTRESS", "", {
		{ BITS_EOT }
	}},
	{0x1167, MSRTYPE_RDWR, MSR2(0,0), "THERM_THRESH_USER_TRIP", "", {
		{ BITS_EOT }
	}},
	{0x1168, MSRTYPE_RDWR, MSR2(0,0), "UNK2", "", {
		{ BITS_EOT }
	}},
	{0x116a, MSRTYPE_RDWR, MSR2(0,0), "UNK3", "", {
		{ BITS_EOT }
	}},
	{0x116b, MSRTYPE_RDWR, MSR2(0,0), "UNK4", "", {
		{ BITS_EOT }
	}},
	{ MSR_EOT }
};
