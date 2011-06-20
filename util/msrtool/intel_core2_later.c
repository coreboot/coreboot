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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "msrtool.h"

int intel_core2_later_probe(const struct targetdef *target) {
	struct cpuid_t *id = cpuid();
	return ((0x6 == id->family)&(0x17 == id->model));
}

const struct msrdef intel_core2_later_msrs[] = {
	{0x17, MSRTYPE_RDWR, MSR2(0,0), "IA32_PLATFORM_ID Register", "Model Specific Platform ID", {
		{ 63, 11, RESERVED },
		{ 52, 3, RESERVED },
		{ 49, 37, RESERVED },
		{ 12, 5, "Maximum Qualified Ratio:", "The maximum allowed bus ratio", PRESENT_DEC, {
			{ BITVAL_EOT }
		}},
		{ 7, 8, RESERVED },
		{ BITS_EOT }
	}},
	{ 0x2a, MSRTYPE_RDWR, MSR2(0,0), "MSR_EBL_CR_POWERON Register", "Processor Hard Power-On Configuration", {
		{ 63, 41, RESERVED },
		{ 26, 5, "Integer Bus Frequency Ratio:", "R/O", PRESENT_DEC, {
			{ BITVAL_EOT }
		}},
		{ 21, 2, "Symmetric Arbitration ID:", "R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 19, 1, RESERVED },
		{ 18, 1, "N/2:", "Non-integer bus ratio", PRESENT_DEC, {
			{ MSR1(0), "Integer ratio" },
			{ MSR1(1), "Non-integer ratio" },
			{ BITVAL_EOT }
		}},
		{ 17, 2, "APIC Cluster ID:", "R/O", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 15, 1, RESERVED },
		{ 14, 1, "1 Mbyte Power on Reset Vector", "R/O", PRESENT_DEC, {
			{ MSR1(0), "4 GBytes Power on Reset Vector" },
			{ MSR1(1), "1 Mbyte Power on Reset Vector" },
			{ BITVAL_EOT }
		}},
		{ 13, 1, RESERVED },
		{ 12, 1, "BINIT# Observation", "R/O", PRESENT_DEC, {
			{ MSR1(0), "BINIT# Observation disabled" },
			{ MSR1(1), "BINIT# Observation enabled" },
			{ BITVAL_EOT }
		}},
		{ 11, 1, "TXT", "Intel TXT Capable Chipset", PRESENT_DEC, {
			{ MSR1(0), "Intel TXT Capable Chipset not present" },
			{ MSR1(1), "Intel TXT Capable Chipset present" },
			{ BITVAL_EOT }
		}},
		{ 10, 1, "MCERR# Observation:", "R/O", PRESENT_DEC, {
			{ MSR1(0), "MCERR# Observation disabled" },
			{ MSR1(1), "MCERR# Observation enabled" },
			{ BITVAL_EOT }
		}},
		{ 9, 1, "Execute BIST", "R/O", PRESENT_DEC, {
			{ MSR1(0), "Execute BIST disabled" },
			{ MSR1(1), "Execute BIST enabled" },
			{ BITVAL_EOT }
		}},
		{ 8, 1, "Output Tri-state", "R/O", PRESENT_DEC, {
			{ MSR1(0), "Output Tri-state disabled" },
			{ MSR1(1), "Output Tri-state enabled" },
			{ BITVAL_EOT }
		}},
		{ 7, 1, "BINIT# Driver Enable", "R/W", PRESENT_DEC, {
			{ MSR1(0), "BINIT# Driver disabled" },
			{ MSR1(1), "BINIT# Driver enabled" },
			{ BITVAL_EOT }
		}},
		{ 6, 2, RESERVED },
		{ 4, 1, "Address parity enable", "R/W", PRESENT_DEC, {
			{ MSR1(0), "Address parity disabled" },
			{ MSR1(1), "Address parity enabled" },
			{ BITVAL_EOT }
		}},
		{ 3, 1, "MCERR# Driver Enable", "R/W", PRESENT_DEC, {
			{ MSR1(0), "MCERR# Driver disabled" },
			{ MSR1(1), "MCERR# Driver enabled" },
			{ BITVAL_EOT }
		}},
		{ 2, 1, "Response error checking enable", "R/W", PRESENT_DEC, {
			{ MSR1(0), "Response Error Checking disabled" },
			{ MSR1(1), "Response Error Checking enabled" },
			{ BITVAL_EOT }
		}},
		{ 1, 1, "Data error checking enable", "R/W", PRESENT_DEC, {
			{ MSR1(0), "Data error checking disabled" },
			{ MSR1(1), "Data error checking enabled" },
			{ BITVAL_EOT }
		}},
		{ 0, 1, RESERVED },
		{ BITS_EOT }
	}},
	{0xcd, MSRTYPE_RDWR, MSR2(0,0), "MSR_FSB_FREQ", "", {
		{ BITS_EOT }
	}},
	{0x11, MSRTYPE_RDWR, MSR2(0,0), "MSR_BBL_CR_CTL3", "", {
		{ BITS_EOT }
	}},
	{0x198, MSRTYPE_RDWR, MSR2(0,0), "IA32_PERF_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x1a0, MSRTYPE_RDWR, MSR2(0,0), "IA32_MISC_ENABLE", "", {
		{ BITS_EOT }
	}},

	// Per core msrs

	{0x0, MSRTYPE_RDWR, MSR2(0,0), "IA32_P5_MC_ADDR", "", {
		{ BITS_EOT }
	}},
	{0x1, MSRTYPE_RDWR, MSR2(0,0), "IA32_P5_MC_TYPE", "", {
		{ BITS_EOT }
	}},
	{0x6, MSRTYPE_RDWR, MSR2(0,0), "IA32_MONITOR_FILTER_SIZE", "", {
		{ BITS_EOT }
	}},
	{0x10, MSRTYPE_RDWR, MSR2(0,0), "IA32_TIME_STEP_COUNTER", "", {
		{ BITS_EOT }
	}},
	{0x1b, MSRTYPE_RDWR, MSR2(0,0), "IA32_APIC_BASE", "", {
		{ BITS_EOT }
	}},
	{0x3a, MSRTYPE_RDWR, MSR2(0,0), "IA32_FEATURE_CONTROL", "", {
		{ BITS_EOT }
	}},
	{0x40, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_0_FROM_IP", "", {
		{ BITS_EOT }
	}},
	{0x41, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_1_FROM_IP", "", {
		{ BITS_EOT }
	}},
	{0x42, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_2_FROM_IP", "", {
		{ BITS_EOT }
	}},
	{0x43, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_3_FROM_IP", "", {
		{ BITS_EOT }
	}},
	{0x60, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_0_TO_LIP", "", {
		{ BITS_EOT }
	}},
	{0x61, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_1_TO_LIP", "", {
		{ BITS_EOT }
	}},
	{0x62, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_2_TO_LIP", "", {
		{ BITS_EOT }
	}},
	{0x63, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_3_TO_LIP", "", {
		{ BITS_EOT }
	}},
	{0x79, MSRTYPE_RDWR, MSR2(0,0), "IA32_BIOS_UPDT_TRIG", "", {
		{ BITS_EOT }
	}},
	{0x8b, MSRTYPE_RDWR, MSR2(0,0), "IA32_BIOS_SIGN_ID", "", {
		{ BITS_EOT }
	}},
	{0xa0, MSRTYPE_RDWR, MSR2(0,0), "MSR_SMRR_PHYS_BASE", "", {
		{ BITS_EOT }
	}},
	{0xa1, MSRTYPE_RDWR, MSR2(0,0), "MSR_SMRR_PHYS_MASK", "", {
		{ BITS_EOT }
	}},
	{0xc1, MSRTYPE_RDWR, MSR2(0,0), "IA32_PMC0", "", {
		{ BITS_EOT }
	}},
	{0xc2, MSRTYPE_RDWR, MSR2(0,0), "IA32_PMC1", "", {
		{ BITS_EOT }
	}},
	{0xe7, MSRTYPE_RDWR, MSR2(0,0), "IA32_MPERF", "", {
		{ BITS_EOT }
	}},
	{0xe8, MSRTYPE_RDWR, MSR2(0,0), "IA32_APERF", "", {
		{ BITS_EOT }
	}},
	{0xfe, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRRCAP", "", {
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
	{0x186, MSRTYPE_RDWR, MSR2(0,0), "IA32_PERFEVTSEL0", "", {
		{ BITS_EOT }
	}},
	{0x187, MSRTYPE_RDWR, MSR2(0,0), "IA32_PERFEVTSEL1", "", {
		{ BITS_EOT }
	}},
	{0x198, MSRTYPE_RDWR, MSR2(0,0), "IA32_PERF_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x199, MSRTYPE_RDWR, MSR2(0,0), "IA32_PERF_CTL", "", {
		{ BITS_EOT }
	}},
	{0x19a, MSRTYPE_RDWR, MSR2(0,0), "IA32_CLOCK_MODULATION", "", {
		{ BITS_EOT }
	}},
	{0x19b, MSRTYPE_RDWR, MSR2(0,0), "IA32_THERM_INTERRUPT", "", {
		{ BITS_EOT }
	}},
	{0x19c, MSRTYPE_RDWR, MSR2(0,0), "IA32_THERM_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x19d, MSRTYPE_RDWR, MSR2(0,0), "MSR_THERM2_CTL", "", {
		{ BITS_EOT }
	}},
	{0x1a0, MSRTYPE_RDWR, MSR2(0,0), "IA32_MISC_ENABLE", "", {
		{ BITS_EOT }
	}},
	{0x1c9, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_TOS", "", {
		{ BITS_EOT }
	}},
	{0x1d9, MSRTYPE_RDWR, MSR2(0,0), "IA32_DEBUGCTL", "", {
		{ BITS_EOT }
	}},
	{0x1dd, MSRTYPE_RDWR, MSR2(0,0), "MSR_LER_FROM_LIP", "", {
		{ BITS_EOT }
	}},
	{0x1de, MSRTYPE_RDWR, MSR2(0,0), "MSR_LER_TO_LIP", "", {
		{ BITS_EOT }
	}},
	{0x200, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYS_BASE0", "", {
		{ BITS_EOT }
	}},
	{0x201, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYS_MASK0", "", {
		{ BITS_EOT }
	}},
	{0x202, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYS_BASE1", "", {
		{ BITS_EOT }
	}},
	{0x203, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYS_MASK1", "", {
		{ BITS_EOT }
	}},
	{0x204, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYS_BASE2", "", {
		{ BITS_EOT }
	}},
	{0x205, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYS_MASK2", "", {
		{ BITS_EOT }
	}},
	{0x206, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYS_BASE3", "", {
		{ BITS_EOT }
	}},
	{0x207, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYS_MASK3", "", {
		{ BITS_EOT }
	}},
	{0x208, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYS_BASE4", "", {
		{ BITS_EOT }
	}},
	{0x209, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYS_MASK4", "", {
		{ BITS_EOT }
	}},
	{0x20a, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYS_BASE5", "", {
		{ BITS_EOT }
	}},
	{0x20b, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYS_MASK5", "", {
		{ BITS_EOT }
	}},
	{0x20c, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYS_BASE6", "", {
		{ BITS_EOT }
	}},
	{0x20d, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYS_MASK6", "", {
		{ BITS_EOT }
	}},
	{0x20e, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYS_BASE7", "", {
		{ BITS_EOT }
	}},
	{0x20f, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYS_MASK7", "", {
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
	{0x277, MSRTYPE_RDWR, MSR2(0,0), "IA32_PAT", "", {
		{ BITS_EOT }
	}},
	{0x2ff, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_DEF_TYPE", "", {
		{ BITS_EOT }
	}},
	{0x309, MSRTYPE_RDWR, MSR2(0,0), "IA32_FIXED_CTR0", "", {
		{ BITS_EOT }
	}},
	{0x30a, MSRTYPE_RDWR, MSR2(0,0), "IA32_FIXED_CTR1", "", {
		{ BITS_EOT }
	}},
	{0x30b, MSRTYPE_RDWR, MSR2(0,0), "IA32_FIXED_CTR2", "", {
		{ BITS_EOT }
	}},
	{0x345, MSRTYPE_RDWR, MSR2(0,0), "IA32_PERF_CAPABILITIES", "", {
		{ BITS_EOT }
	}},
	{0x38d, MSRTYPE_RDWR, MSR2(0,0), "IA32_FIXED_CTR_CTRL", "", {
		{ BITS_EOT }
	}},
	{0x38e, MSRTYPE_RDWR, MSR2(0,0), "IA32_PERF_GLOBAL_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x38f, MSRTYPE_RDWR, MSR2(0,0), "IA32_PERF_GLOBAL_CTL", "", {
		{ BITS_EOT }
	}},
	{0x390, MSRTYPE_RDWR, MSR2(0,0), "IA32_PERF_GLOBAL_OVF_CTL", "", {
		{ BITS_EOT }
	}},
	{0x3f1, MSRTYPE_RDWR, MSR2(0,0), "IA32_PEBS_ENABLE", "", {
		{ BITS_EOT }
	}},
	{0x400, MSRTYPE_RDWR, MSR2(0,0), "IA32_MCO_CTL", "", {
		{ BITS_EOT }
	}},
	{0x401, MSRTYPE_RDWR, MSR2(0,0), "IA32_MCO_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x402, MSRTYPE_RDWR, MSR2(0,0), "IA32_MCO_ADDR", "", {
		{ BITS_EOT }
	}},
	{0x403, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC0_MISC", "", {
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
	{0x407, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC1_MISC", "", {
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
	{0x40b, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC2_MISC", "", {
		{ BITS_EOT }
	}},
	{0x40c, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC4_CTL", "", {
		{ BITS_EOT }
	}},
	{0x40d, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC4_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x40e, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC4_ADDR", "", {
		{ BITS_EOT }
	}},
	{0x40f, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC4_MISC", "", {
		{ BITS_EOT }
	}},
	{0x410, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC3_CTL", "", {
		{ BITS_EOT }
	}},
	{0x411, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC3_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x412, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC3_ADDR", "", {
		{ BITS_EOT }
	}},
	{0x413, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC3_MISC", "", {
		{ BITS_EOT }
	}},
	{0x414, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC5_CTL", "", {
		{ BITS_EOT }
	}},
	{0x415, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC5_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x416, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC5_ADDR", "", {
		{ BITS_EOT }
	}},
	{0x417, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC5_MISC", "", {
		{ BITS_EOT }
	}},
	{0x418, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC6_CTL", "", {
		{ BITS_EOT }
	}},
	{0x419, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC6_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x480, MSRTYPE_RDWR, MSR2(0,0), "IA32_VMX_BASIC", "", {
		{ BITS_EOT }
	}},
	{0x481, MSRTYPE_RDWR, MSR2(0,0), "IA32_PINBASED_CTLS", "", {
		{ BITS_EOT }
	}},
	{0x482, MSRTYPE_RDWR, MSR2(0,0), "IA32_PROCBASED_CTLS", "", {
		{ BITS_EOT }
	}},
	{0x483, MSRTYPE_RDWR, MSR2(0,0), "IA32_VMX_EXIT_CTLS", "", {
		{ BITS_EOT }
	}},
	{0x484, MSRTYPE_RDWR, MSR2(0,0), "IA32_VMX_ENTRY_CTLS", "", {
		{ BITS_EOT }
	}},
	{0x485, MSRTYPE_RDWR, MSR2(0,0), "IA32_VMX_MISC", "", {
		{ BITS_EOT }
	}},
	{0x486, MSRTYPE_RDWR, MSR2(0,0), "IA32_VMX_CR0_FIXED0", "", {
		{ BITS_EOT }
	}},
	{0x487, MSRTYPE_RDWR, MSR2(0,0), "IA32_VMX_CR0_FIXED1", "", {
		{ BITS_EOT }
	}},
	{0x488, MSRTYPE_RDWR, MSR2(0,0), "IA32_VMX_CR4_FIXED0", "", {
		{ BITS_EOT }
	}},
	{0x489, MSRTYPE_RDWR, MSR2(0,0), "IA32_VMX_CR4_FIXED1", "", {
		{ BITS_EOT }
	}},
	{0x48a, MSRTYPE_RDWR, MSR2(0,0), "IA32_VMX_VMCS_ENUM", "", {
		{ BITS_EOT }
	}},
	{0x48b, MSRTYPE_RDWR, MSR2(0,0), "IA32_VMX_PROCBASED_CTLS2", "", {
		{ BITS_EOT }
	}},
	{0x600, MSRTYPE_RDWR, MSR2(0,0), "IA32_DS_AREA", "", {
		{ BITS_EOT }
	}},
	{0x107cc, MSRTYPE_RDWR, MSR2(0,0), "MSR_EMON_L3_CTR_CTL0", "", {
		{ BITS_EOT }
	}},
	{0x107cd, MSRTYPE_RDWR, MSR2(0,0), "MSR_EMON_L3_CTR_CTL1", "", {
		{ BITS_EOT }
	}},
	{0x107ce, MSRTYPE_RDWR, MSR2(0,0), "MSR_EMON_L3_CTR_CTL2", "", {
		{ BITS_EOT }
	}},
	{0x107cf, MSRTYPE_RDWR, MSR2(0,0), "MSR_EMON_L3_CTR_CTL3", "", {
		{ BITS_EOT }
	}},
	{0x107d0, MSRTYPE_RDWR, MSR2(0,0), "MSR_EMON_L3_CTR_CTL4", "", {
		{ BITS_EOT }
	}},
	{0x107d1, MSRTYPE_RDWR, MSR2(0,0), "MSR_EMON_L3_CTR_CTL5", "", {
		{ BITS_EOT }
	}},
	{0x107d2, MSRTYPE_RDWR, MSR2(0,0), "MSR_EMON_L3_CTR_CTL6", "", {
		{ BITS_EOT }
	}},
	{0x107d3, MSRTYPE_RDWR, MSR2(0,0), "MSR_EMON_L3_CTR_CTL7", "", {
		{ BITS_EOT }
	}},
	{0x107d8, MSRTYPE_RDWR, MSR2(0,0), "MSR_EMON_L3_GL_CTL", "", {
		{ BITS_EOT }
	}},
	{0xc0000080, MSRTYPE_RDWR, MSR2(0,0), "IA32_EFER", "", {
		{ BITS_EOT }
	}},
	{0xc0000081, MSRTYPE_RDWR, MSR2(0,0), "IA32_STAR", "", {
		{ BITS_EOT }
	}},
	{0xc0000082, MSRTYPE_RDWR, MSR2(0,0), "IA32_LSTAR", "", {
		{ BITS_EOT }
	}},
	{0xc0000084, MSRTYPE_RDWR, MSR2(0,0), "IA32_FMASK", "", {
		{ BITS_EOT }
	}},
	{0xc0000100, MSRTYPE_RDWR, MSR2(0,0), "IA32_FS_BASE", "", {
		{ BITS_EOT }
	}},
	{0xc0000101, MSRTYPE_RDWR, MSR2(0,0), "IA32_GS_BASE", "", {
		{ BITS_EOT }
	}},
	{0xc0000102, MSRTYPE_RDWR, MSR2(0,0), "IA32_KERNEL_GS_BASE", "", {
		{ BITS_EOT }
	}},
	{ MSR_EOT }
};


