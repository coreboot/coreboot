/*
 * This file is part of msrtool.
 *
 * Copyright (C) 2013 Olivier Langlois <olivier@olivierlanglois.net>
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

int intel_atom_probe(const struct targetdef *target, const struct cpuid_t *id) {
	return ((VENDOR_INTEL == id->vendor) &&
		(0x6 == id->family) &&
		(0x1c == id->model));
}

const struct msrdef intel_atom_msrs[] = {
	{0x0, MSRTYPE_RDWR, MSR2(0,0), "IA32_P5_MC_ADDR", "Pentium Processor\
			 Machine-Check Exception Address", {
		{ BITS_EOT }
	}},
	{0x1, MSRTYPE_RDWR, MSR2(0,0), "IA32_P5_MC_TYPE", "Pentium Processor\
			 Machine-Check Exception Type", {
		{ BITS_EOT }
	}},
	{0x10, MSRTYPE_RDWR, MSR2(0,0), "IA32_TIME_STEP_COUNTER", "TSC", {
		{ BITS_EOT }
	}},
	{0x17, MSRTYPE_RDWR, MSR2(0,0), "IA32_PLATFORM_ID", "", {
		{ BITS_EOT }
	}},
	{0x2a, MSRTYPE_RDWR, MSR2(0,0), "MSR_EBL_CR_POWERON", "", {
		{ BITS_EOT }
	}},
	{0xcd, MSRTYPE_RDONLY, MSR2(0,0), "MSR_FSB_FREQ", "Scaleable Bus Speed", {
		{ BITS_EOT }
	}},
	{0xfe, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRRCAP", "", {
		{ BITS_EOT }
	}},
	{0x11e, MSRTYPE_RDWR, MSR2(0,0), "MSR_BBL_CR_CTL3", "", {
		{ BITS_EOT }
	}},
	{0x198, MSRTYPE_RDWR, MSR2(0,0), "IA32_PERF_STATUS", "", {
		{ 63, 19, RESERVED },
		{ 44, 5,  "Maximum Bus Ratio", "R/O", PRESENT_DEC, {
			{ BITVAL_EOT }
		}},
		{ 39, 24, RESERVED },
		{ 15, 16, "Current Performance State Value", "R/O", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x19d, MSRTYPE_RDWR, MSR2(0,0), "MSR_THERM2_CTL", "", {
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
#if 0
	{0x20e, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYS_BASE7", "", {
		{ BITS_EOT }
	}},
	{0x20f, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYS_MASK7", "", {
		{ BITS_EOT }
	}},
#endif
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
	/* if CPUID.01H: ECX[15] = 1 */
	{0x345, MSRTYPE_RDONLY, MSR2(0,0), "IA32_PERF_CAPABILITIES", "", {
	/* Additional info available at Section 17.4.1 of
	 * Intel 64 and IA-32 Architecures Software Developer's
	 * Manual, Volume 3.
	 */
		{ 63, 50, RESERVED },
		{ 13, 1, "Counter width", "R/O", PRESENT_BIN, {
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "Full width of counter writable via IA32_A_PMCx" },
			{ BITVAL_EOT }
		}},
		{ 12, 1, "SMM_FREEZE", "R/O", PRESENT_BIN, {
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "Freeze while SMM is supported" },
			{ BITVAL_EOT }
		}},
		{ 11, 4, "PEBS_REC_FORMAT", "R/O", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 7, 1, "PEBSSaveArchRegs", "R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 6, 1, "PEBS Record Format", "R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 5, 6, "LBR Format", "R/O", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
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

/* ==========================================================================
 *                             Per core MSRs
 * ==========================================================================
 */

	{0x6, MSRTYPE_RDWR, MSR2(0,0), "IA32_MONITOR_FILTER_SIZE", "", {
		{ BITS_EOT }
	}},
	{0x10, MSRTYPE_RDWR, MSR2(0,0), "IA32_TIME_STEP_COUNTER", "TSC", {
		{ BITS_EOT }
	}},
	{0x1b, MSRTYPE_RDWR, MSR2(0,0), "IA32_APIC_BASE", "APIC BASE", {
		/* In Intel's manual there is MAXPHYWID,
		 * which determine index of highest bit of
		 * APIC Base itself, so marking it as
		 * 'RESERVED'.
		 */
		{ 63, 52, RESERVED },
		{ 11, 1, "APIC Global Enable", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 10, 1, "x2APIC mode", "R/W", PRESENT_BIN, {
			{ MSR1(0), "x2APIC mode is disabled" },
			{ MSR1(1), "x2APIC mode is enabled" },
			{ BITVAL_EOT }
		}},
		{ 9, 1, RESERVED },
		{ 8, 1, "BSP Flag", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 7, 8, RESERVED },
		{ BITS_EOT }
	}},
	/* if CPUID.01H: ECX[bit 5 or bit 6] = 1 */
	{0x3a, MSRTYPE_RDWR, MSR2(0,0), "IA32_FEATURE_CONTROL",
			"Control features in Intel 64Processor", {
		{ 63, 48, RESERVED },
		/* if CPUID.01H: ECX[6] = 1 */
		{ 15, 1, "SENTER Global Enable", "R/WL", PRESENT_BIN, {
			{ MSR1(0), "SENTER leaf functions are disabled" },
			{ MSR1(1), "SENTER leaf functions are enabled" },
			{ BITVAL_EOT }
		}},
		/* if CPUID.01H: ECX[6] = 1 */
		{ 14, 7, "SENTER Local Function Enables", "R/WL", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 7, 5, RESERVED },
		/* if CPUID.01H: ECX[5 or 6] = 1 */
		{ 2, 1, "VMX outside of SMX operation", "R/WL", PRESENT_BIN, {
			/* This bit enables VMX for system executive
			 * that do not require SMX.
			 */
			{ MSR1(0), "VMX outside of SMX operation disabled" },
			{ MSR1(1), "VMX outside of SMX operation enabled" },
			{ BITVAL_EOT }
		}},
		{ 1, 1, "VMX inside of SMX operation", "R/WL", PRESENT_BIN, {
			/* This bit enables a system executive to use
			 * VMX in conjunction with SMX to support Intel
			 * Trusted Execution Technology.
			 */
			{ MSR1(0), "VMX inside of SMX operation disabled" },
			{ MSR1(1), "VMX inside of SMX operation enabled" },
			{ BITVAL_EOT }
		}},
		/* if CPUID.01H: ECX[5 or 6] = 1 */
		{ 0, 1, "Lock bit", "R/WO", PRESENT_BIN, {
			/* Once the Lock bit is set, the contents
			 * of this register cannot be modified.
			 * Therefore the lock bit must be set after
			 * configuring support for Intel Virtualization
			 * Technology and prior transferring control
			 * to an Option ROM or bootloader. Hence, once
			 * the lock bit is set, the entire IA32_FEATURE_CONTROL_MSR
			 * contents are preserved across RESET when
			 * PWRGOOD it not deasserted.
			 */
			{ MSR1(0), "IA32_FEATURE_CONTROL MSR can be modified" },
			{ MSR1(1), "IA32_FEATURE_CONTROL MSR cannot be modified" },
			{ BITVAL_EOT }
		}},
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
	{0x44, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_4_FROM_IP", "", {
		{ BITS_EOT }
	}},
	{0x45, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_5_FROM_IP", "", {
		{ BITS_EOT }
	}},
	{0x46, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_6_FROM_IP", "", {
		{ BITS_EOT }
	}},
	{0x47, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_7_FROM_IP", "", {
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
	{0x64, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_4_TO_LIP", "", {
		{ BITS_EOT }
	}},
	{0x65, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_5_TO_LIP", "", {
		{ BITS_EOT }
	}},
	{0x66, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_6_TO_LIP", "", {
		{ BITS_EOT }
	}},
	{0x67, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_7_TO_LIP", "", {
		{ BITS_EOT }
	}},
	{0x8b, MSRTYPE_RDWR, MSR2(0,0), "IA32_BIOS_SIGN_ID",
			"BIOS Update Signature ID (RO)", {
		{ BITS_EOT }
	}},
	{0xc1, MSRTYPE_RDWR, MSR2(0,0), "IA32_PMC0",
			"Performance counter register", {
		{ BITS_EOT }
	}},
	{0xc2, MSRTYPE_RDWR, MSR2(0,0), "IA32_PMC1",
			"Performance counter register", {
		{ BITS_EOT }
	}},
	{0xe7, MSRTYPE_RDWR, MSR2(0,0), "IA32_MPERF", "", {
		{ BITS_EOT }
	}},
	{0xe8, MSRTYPE_RDWR, MSR2(0,0), "IA32_APERF", "", {
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
	{0x17a, MSRTYPE_RDWR, MSR2(0,0), "IA32_MCG_STATUS", "", {
		{ 63, 61, RESERVED },
		{ 2, 1, "MCIP", "R/W", PRESENT_BIN, {
			/* When set, bit indicates that a machine check has been
			 * generated. If a second machine check is detected while
			 * this bit is still set, the processor enters a shutdown state.
			 * Software should write this bit to 0 after processing
			 * a machine check exception.
			 */
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "Machine check has been generated" },
			{ BITVAL_EOT }
		}},
		{ 1, 1, "EPIV", "R/W", PRESENT_BIN, {
			/* When set, bit indicates that the instruction addressed
			 * by the instruction pointer pushed on the stack (when
			 * the machine check was generated) is directly associated
			 * with the error
			 */
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "Instruction addressed directly associated with the error" },
			{ BITVAL_EOT }
		}},
		{ 0, 1, "RIPV", "R/W", PRESENT_BIN, {
			/* When set, bit indicates that the instruction addressed
			 * by the instruction pointer pushed on the stack (when
			 * the machine check was generated) can be used to restart
			 * the program. If cleared, the program cannot be reliably restarted
			 */
			{ MSR1(0), "Program cannot be reliably restarted" },
			{ MSR1(1), "Instruction addressed can be used to restart the program" },
			{ BITVAL_EOT }
		}},
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
	{0x199, MSRTYPE_RDWR, MSR2(0,0), "IA32_PERF_CTL", "", {
		{ BITS_EOT }
	}},
	{0x19a, MSRTYPE_RDWR, MSR2(0,0), "IA32_CLOCK_MODULATION",
			"Clock Modulation", {
		{ 63, 59, RESERVED },
		{ 4, 1, "On demand Clock Modulation", "R/W", PRESENT_BIN, {
			{ MSR1(0), "On demand Clock Modulation is disabled" },
			{ MSR1(1), "On demand Clock Modulation is enabled" },
			{ BITVAL_EOT }
		}},
		{ 3, 3, "On demand Clock Modulation Duty Cycle", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 0, 1, RESERVED },
		{ BITS_EOT }
	}},
	{0x19b, MSRTYPE_RDWR, MSR2(0,0), "IA32_THERM_INTERRUPT",
			"Thermal Interrupt Control", {
		{ BITS_EOT }
	}},
	{0x19c, MSRTYPE_RDWR, MSR2(0,0), "IA32_THERM_STATUS",
			"Thermal Monitor Status", {
		{ BITS_EOT }
	}},
	{0x1a0, MSRTYPE_RDWR, MSR2(0,0), "IA32_MISC_ENABLE",
			"Enable miscellaneous processor features", {
		{ 63, 25, RESERVED },
		/* Note: [38] bit using for whole package,
		 * while some other bits can be Core or Thread
		 * specific.
		 */
		{ 38, 1, "Turbo Mode", "R/W", PRESENT_BIN, {
			/* When set to a 0 on processors that support IDA,
			 * CPUID.06H: EAX[1] reports the processor's
			 * support of turbo mode is enabled.
			 */
			{ MSR1(0), "Turbo Mode enabled" },
			/* When set 1 on processors that support Intel Turbo Boost
			 * technology, the turbo mode feature is disabled and
			 * the IDA_Enable feature flag will be clear (CPUID.06H: EAX[1]=0).
			 */
			{ MSR1(1), "Turbo Mode disabled" },
			{ BITVAL_EOT }
			/* Note: the power-on default value is used by BIOS to detect
			 * hardware support of turbo mode. If power-on default value is 1,
			 * turbo mode is available in the processor. If power-on default
			 * value is 0, turbo mode not available.
			 */
		}},
		{ 37, 3, RESERVED },
		{ 34, 1, "XD Bit Disable", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 33, 10, RESERVED },
		{ 23, 1, "xTPR Message Disable", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 22, 1, "Limit CPUID Maxval", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 21, 3, RESERVED },
		{ 18, 1, "Enable Monitor FSM", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 17, 1, "UNDOCUMENTED", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		/* Note: [16] bit using for whole package,
		 * while some other bits can be Core or Thread
		 * specific.
		 */
		{ 16, 1, "Enhanced Intel SpeedStep Technology Enable", "R/W",
				PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 15, 3, RESERVED },
		{ 12, 1, "Precise Event Based Sampling Unavailable", "R/O",
				PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 11, 1, "Branch Trace Storage Unavailable", "R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 10, 3, RESERVED },
		{ 7, 1, "Performance Monitoring Available", "R", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 6, 3, RESERVED },
		{ 3, 1, "Automatic Thermal Control Circuit Enable", "R/W"
				, PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 2, 2, RESERVED },
		{ 0, 1, "Fast-Strings Enable", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x1c9, MSRTYPE_RDONLY, MSR2(0,0), "MSR_LASTBRANCH_TOS",
		"Last Branch Record Stack TOS", {
	/* Contains an index (bits 0-3) that points to the MSR containing
	 * the most recent branch record. See also MSR_LASTBRANCH_0_FROM_IP (0x680).
	 */
		{ BITS_EOT }
	}},
	{0x1d9, MSRTYPE_RDWR, MSR2(0,0), "IA32_DEBUGCTL",
		"Debug/Trace/Profile Resource Control", {
	/* (MSR_DEBUGCTTLA, MSR_DEBUGCTLB) */
		{ 63, 49, RESERVED },
		/* Only if IA32_PERF_CAPABILITIES[12] = 1 */
		{ 14, 1, "FREEZE_WHILE_SMM", "R/O", PRESENT_BIN, {
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "Freeze perfmon and trace messages while in SMM" },
			{ BITVAL_EOT }
		}},
		{ 13, 1, "ENABLE_UNCORE_PMI", "R/O", PRESENT_BIN, {
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "Logical processor can receive and generate PMI \
				on behalf of the uncore" },
			{ BITVAL_EOT }
		}},
		/* Only if CPUID.01H: ECX[15] = 1 and CPUID.0AH: EAX[7:0]>1 */
		{ 12, 1, "FREEZE_PERFMON_ON_PMI", "R/O", PRESENT_BIN, {
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "Each ENABLE bit of the global counter control MSR \
				are frozen (address 0x3bf) on PMI request" },
			{ BITVAL_EOT }
		}},
		/* Only if CPUID.01H: ECX[15] = 1 and CPUID.0AH: EAX[7:0]>1 */
		{ 11, 1, "FREEZE_LBRS_ON_PMI", "R/O", PRESENT_BIN, {
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "LBR stack is frozen on PMI request" },
			{ BITVAL_EOT }
		}},
		{ 10, 1, "BTS_OFF_USR", "R/O", PRESENT_BIN, {
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "BTS or BTM is skipped if CPL > 0" },
			{ BITVAL_EOT }
		}},
		{ 9, 1, "BTS_OFF_OS", "R/O", PRESENT_BIN, {
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "BTS or BTM is skipped if CPL = 0" },
			{ BITVAL_EOT }
		}},
		{ 8, 1, "BTINT", "R/O", PRESENT_BIN, {
			{ MSR1(0), "BTMs are logged in a BTS buffer in circular fashion" },
			{ MSR1(1), "An interrupt is generated by the BTS facility \
				when the BTS buffer is full" },
			{ BITVAL_EOT }
		}},
		{ 7, 1, "BTS", "R/O", PRESENT_BIN, {
			{ MSR1(0), "Logging of BTMs (branch trace messages) \
				in BTS buffer is disabled" },
			{ MSR1(1), "Logging of BTMs (branch trace messages) \
				in BTS buffer is enabled" },
			{ BITVAL_EOT }
		}},
		{ 6, 1, "TR", "R/O", PRESENT_BIN, {
			{ MSR1(0), "Branch trace messages are disabled" },
			{ MSR1(1), "Branch trace messages are enabled" },
			{ BITVAL_EOT }
		}},
		{ 5, 4, RESERVED },
		{ 1, 1, "BTF", "R/O", PRESENT_BIN, {
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "Enabled treating EFLAGS.TF as single-step on \
				branches instead of single-step on instructions" },
			{ BITVAL_EOT }
		}},
		{ 0, 1, "LBR", "R/O", PRESENT_BIN, {
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "Enabled recording a running trace of the most \
				recent branches taken by the processor in the LBR stack" },
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x1dd, MSRTYPE_RDONLY, MSR2(0,0), "MSR_LER_FROM_LIP",
		"Last Exception Record From Linear IP", {
	/* Contains a pointer to the last branch instruction
	 * that the processor executed prior to the last exception
	 * that was generated or the last interrupt that was handled.
	 */
		{ BITS_EOT }
	}},
	{0x1de, MSRTYPE_RDONLY, MSR2(0,0), "MSR_LER_TO_LIP",
		"Last Exception Record To Linear IP", {
	/* This area contains a pointer to the target of the
	 * last branch instruction that the processor executed
	 * prior to the last exception that was generated or
	 * the last interrupt that was handled
	 */
		{ BITS_EOT }
	}},
	{0x277, MSRTYPE_RDWR, MSR2(0,0), "IA32_PAT", "IA32_PAT", {
		{ 63, 5, RESERVED },
		{ 58, 3, "PA7", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 55, 5, RESERVED },
		{ 40, 3, "PA6", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 47, 5, RESERVED },
		{ 42, 3, "PA5", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 39, 5, RESERVED },
		{ 34, 3, "PA4", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 31, 5, RESERVED },
		{ 26, 3, "PA3", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 23, 5, RESERVED },
		{ 18, 3, "PA2", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 15, 5, RESERVED },
		{ 10, 3, "PA1", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 7, 5, RESERVED },
		{ 2, 3, "PA0", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	/* if CPUID.0AH: EDX[4:0] > 0 */
	{0x309, MSRTYPE_RDWR, MSR2(0,0), "IA32_FIXED_CTR0", "Fixed-Function \
			Performance Counter Register 0: Counts Instr_Retired.Any", {
		/* Also known as MSR_PERF_FIXED_CTR0 */
		{ BITS_EOT }
	}},
	/* if CPUID.0AH: EDX[4:0] > 1 */
	{0x30a, MSRTYPE_RDWR, MSR2(0,0), "IA32_FIXED_CTR1", "Fixed-Function \
			Performance Counter Register 1: Counts CPU_CLK_Unhalted.Core ", {
		/* Also known as MSR_PERF_FIXED_CTR1 */
		{ BITS_EOT }
	}},
	/* if CPUID.0AH: EDX[4:0] > 2 */
	{0x30b, MSRTYPE_RDWR, MSR2(0,0), "IA32_FIXED_CTR2", "Fixed-Function \
			Performance Counter Register 2: Counts CPU_CLK_Unhalted.Ref", {
		/* Also known as MSR_PERF_FIXED_CTR2 */
		{ BITS_EOT }
	}},
	/* if CPUID.0AH: EAX[7:0] > 1*/
	{0x38d, MSRTYPE_RDWR, MSR2(0,0), "IA32_FIXED_CTR_CTRL",
		"Fixed-Function-Counter Control Register", {
	/* Also known as MSR_PERF_FIXED_CTR_CTRL.
	 * Counter increments while the results of ANDing respective enable bit
	 * in IA32_PERF_GLOBAL_CTRL with the corresponding OS or USR bits
	 * in this MSR is true. */
		{ 63, 52, RESERVED },
		{ 11, 1, "EN2_PMI", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "PMI when fixed counter 2 overflows is enabled" },
			{ BITVAL_EOT }
		}},
		/* if CPUID.0AH EAX[7:0] > 2 */
		{ 10, 1, "AnyThread 2", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Counter only increments the associated event \
				conditions occurring in the logical processor which programmed the MSR" },
			{ MSR1(1), "Counting the associated event conditions \
				occurring across all logical processors sharing a processor core" },
			{ BITVAL_EOT }
		}},
		{ 9, 1, "EN2_Usr", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "Fixed counter 2 is enabled to count while CPL > 0" },
			{ BITVAL_EOT }
		}},
		{ 8, 1, "EN2_OS", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "Fixed counter 2 is enabled to count while CPL = 0" },
			{ BITVAL_EOT }
		}},
		{ 7, 1, "EN1_PMI", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "PMI when fixed counter 1 overflows is enabled" },
			{ BITVAL_EOT }
		}},
		/* if CPUID.0AH: EAX[7:0] > 2 */
		{ 6, 1, "AnyThread 1", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Counter only increments the associated event \
				conditions occurring in the logical processor which programmed the MSR" },
			{ MSR1(1), "Counting the associated event conditions \
				occurring across all logical processors sharing a processor core" },
			{ BITVAL_EOT }
		}},
		{ 5, 1, "EN1_Usr", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "Fixed counter 1 is enabled to count while CPL > 0" },
			{ BITVAL_EOT }
		}},
		{ 4, 1, "EN1_OS", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "Fixed counter 1 is enabled to count while CPL = 0" },
			{ BITVAL_EOT }
		}},
		{ 3, 1, "EN0_PMI", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "PMI when fixed counter 0 overflows is enabled" },
			{ BITVAL_EOT }
		}},
		/* if CPUID.0AH: EAX[7:0] > 2 */
		{ 2, 1, "AnyThread 0", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Counter only increments the associated event \
				conditions occurring in the logical processor which programmed the MSR" },
			{ MSR1(1), "Counting the associated event conditions \
				occurring across all logical processors sharing a processor core" },
			{ BITVAL_EOT }
		}},
		{ 1, 1, "EN0_Usr", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "Fixed counter 0 is enabled to count while CPL > 0" },
			{ BITVAL_EOT }
		}},
		{ 0, 1, "EN0_OS", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "Fixed counter 0 is enabled to count while CPL = 0" },
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	/* if CPUID.0AH: EAX[7:0] > 0 */
	{0x38e, MSRTYPE_RDONLY, MSR2(0,0), "IA32_PERF_GLOBAL_STATUS",
			"Global Performance Counter Status", {
	/* Also known as MSR_PERF_GLOBAL_STATUS */
		/* if CPUID.0AH: EAX[7:0] > 0 */
		{ 63, 1, "CondChg: Status bits of this register has changed",
				"R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		/* if CPUID.0AH: EAX[7:0] > 0 */
		{ 62, 1, "OvfBuf: DS SAVE area Buffer overflow status",
				"R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		/* if CPUID.0AH: EAX[7:0] > 2 */
		{ 61, 1, "Ovf_Uncore: Uncore counter overflow status",
				"R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 60, 26, RESERVED },
		/* if CPUID.0AH: EAX[7:0] > 1 */
		{ 34, 1, "Ovf_FixedCtr2: Overflow status of IA32_FIXED_CTR2",
				"R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		/* if CPUID.0AH: EAX[7:0] > 1 */
		{ 33, 1, "Ovf_FixedCtr1: Overflow status of IA32_FIXED_CTR1",
				"R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		/* if CPUID.0AH: EAX[7:0] > 1 */
		{ 32, 1, "Ovf_FixedCtr0: Overflow status of IA32_FIXED_CTR0",
				"R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 31, 28, RESERVED },
		/* presented only in 06_2EH Nehalem model */
		{ 3, 1, "Ovf_PMC3: Overflow status of IA32_PMC3", "R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		/* presented only in 06_2EH Nehalem model */
		{ 2, 1, "Ovf_PMC2: Overflow status of IA32_PMC2", "R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		/* if CPUID.0AH: EAX[7:0] > 0 */
		{ 1, 1, "Ovf_PMC1: Overflow status of IA32_PMC1", "R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		/* if CPUID.0AH: EAX[7:0] > 0 */
		{ 0, 1, "Ovf_PMC0: Overflow status of IA32_PMC0", "R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	/* if CPUID.0AH: EAX[7:0] > 0 */
	{0x38f, MSRTYPE_RDWR, MSR2(0,0), "IA32_PERF_GLOBAL_CTL",
		"Global Performance Counter Control", {
	/* Counter increments while the result of ANDing respective
	 * enable bit in this MSR with corresponding OS or USR bits
	 * in general-purpose or fixed counter control MSR is true.
	 */
		{ 63, 29, RESERVED },
		/* if CPUID.0AH: EAX[7:0] > 1 */
		{ 34, 1, "EN_FIXED_CTR2", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		/* if CPUID.0AH: EAX[7:0] > 1 */
		{ 33, 1, "EN_FIXED_CTR1", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		/* if CPUID.0AH: EAX[7:0] > 1 */
		{ 32, 1, "EN_FIXED_CTR0", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 31, 30, RESERVED },
		/* if CPUID.0AH: EAX[7:0] > 0 */
		{ 1, 1, "EN_PMC1", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		/* if CPUID.0AH: EAX[7:0] > 0 */
		{ 0, 1, "EN_PMC0", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	/* if CPUID.0AH: EAX[7:0] > 0 */
	{0x390, MSRTYPE_RDWR, MSR2(0,0), "IA32_PERF_GLOBAL_OVF_CTL",
			"Global Performance Counter Overflow Control", {
		/* if CPUID.0AH: EAX[7:0] > 0 */
		{ 63, 1, "Clear CondChg bit", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		/* if CPUID.0AH: EAX[7:0] > 0 */
		{ 62, 1, "Clear OvfBuf bit", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		/* Presented only in 06_2EH Nehalem model */
		{ 61, 1, "Clear Ovf_Uncore bit", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 60, 26, RESERVED },
		/* if CPUID.0AH: EAX[7:0] > 1 */
		{ 34, 1, "Clear Ovf_FIXED_CTR2 bit", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		/* if CPUID.0AH: EAX[7:0] > 1 */
		{ 33, 1, "Clear Ovf_FIXED_CTR1 bit", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		/* if CPUID.0AH: EAX[7:0] > 1 */
		{ 32, 1, "Clear Ovf_FIXED_CTR0 bit", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 31, 30, RESERVED },
		/* if CPUID.0AH: EAX[7:0] > 0 */
		{ 1, 1, "Clear Ovf_PMC1 bit", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		/* if CPUID.0AH: EAX[7:0] > 0 */
		{ 0, 1, "Clear Ovf_PMC0 bit", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	/* See Section 18.6.1.1 of Intel 64 and IA-32 Architectures
	 * Software Developer's Manual, Volume 3,
	 * "Precise Event Based Sampling (PEBS)".
	 */
	{0x3f1, MSRTYPE_RDWR, MSR2(0,0), "IA32_PEBS_ENABLE", "PEBS Control", {
		{ 63, 28, RESERVED },
		{ 35, 1, "Load Latency on IA32_PMC3", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Disabled" },
			{ MSR1(1), "Enabled" },
			{ BITVAL_EOT }
		}},
		{ 34, 1, "Load Latency on IA32_PMC2", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Disabled" },
			{ MSR1(1), "Enabled" },
			{ BITVAL_EOT }
		}},
		{ 33, 1, "Load Latency on IA32_PMC1", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Disabled" },
			{ MSR1(1), "Enabled" },
			{ BITVAL_EOT }
		}},
		{ 32, 1, "Load Latency on IA32_PMC0", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Disabled" },
			{ MSR1(1), "Enabled" },
			{ BITVAL_EOT }
		}},
		{ 31, 28, RESERVED },
		{ 3, 1, "PEBS on IA32_PMC3", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Disabled" },
			{ MSR1(1), "Enabled" },
			{ BITVAL_EOT }
		}},
		{ 2, 1, "PEBS on IA32_PMC2", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Disabled" },
			{ MSR1(1), "Enabled" },
			{ BITVAL_EOT }
		}},
		{ 1, 1, "PEBS on IA32_PMC1", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Disabled" },
			{ MSR1(1), "Enabled" },
			{ BITVAL_EOT }
		}},
		{ 0, 1, "PEBS on IA32_PMC0", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Disabled" },
			{ MSR1(1), "Enabled" },
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
#if 0
	{0x480, MSRTYPE_RDONLY, MSR2(0,0), "IA32_VMX_BASIC",
			"Reporting Register of Basic VMX Capabilities", {
		/* Additional info available at
		 * Appendix A.1, "Basic VMX Information" */
		{ 63, 10, RESERVED },
		{ 53, 4, "Memory type for VMREAD and VMWRITE", "R/O", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 49, 1, "Support of dual-treatment of system-management functions",
				"R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 48, 1, "Enable full linear address access", "R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 47, 3, RESERVED },
		{ 44, 13, "VMXON region allocation size", "R/O", PRESENT_DEC, {
			{ BITVAL_EOT }
		}},
		{ 31, 32, "VMCS Revision Identifier", "R/O", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x481, MSRTYPE_RDONLY, MSR2(0,0), "IA32_PINBASED_CTLS",
			"Capability Reporting Register of \
			Pin-based VM-execution Controls", {
		/* Additional info available at Appendix A.3,
		 * "VM-Execution Controls" */
		{ BITS_EOT }
	}},
	{0x482, MSRTYPE_RDONLY, MSR2(0,0), "IA32_PROCBASED_CTLS",
			"Capability Reporting Register of \
			Primary Processor-based VM-execution Controls", {
		/* Additional info available at Appendix A.3,
		 * "VM-Execution Controls" */
		{ BITS_EOT }
	}},
	{0x483, MSRTYPE_RDONLY, MSR2(0,0), "IA32_VMX_EXIT_CTLS",
			"Capability Reporting Register of VM-exit Controls", {
		/* Additional info available at Appendix A.4,
		 * "VM-Exit Controls" */
		{ BITS_EOT }
	}},
	{0x484, MSRTYPE_RDONLY, MSR2(0,0), "IA32_VMX_ENTRY_CTLS",
			"Capability Reporting Register of VM-entry Controls", {
		/* Additional info available at Appendix A.5,
		 * "VM-Entry Controls" */
		{ BITS_EOT }
	}},
	{0x485, MSRTYPE_RDONLY, MSR2(0,0), "IA32_VMX_MISC",
			"Reporting Register of Miscellaneous VMX Capabilities", {
		/* Additional info available at Appendix A.6,
		 * "Miscellaneous Data" */
		{ BITS_EOT }
	}},
	{0x486, MSRTYPE_RDONLY, MSR2(0,0), "IA32_VMX_CR0_FIXED0",
			"Capability Reporting Register of CR0 Bits Fixed to 0", {
		/* Additional info available at Appendix A.7,
		 * "VMX-Fixed Bits in CR0" */
		{ BITS_EOT }
	}},
	{0x487, MSRTYPE_RDONLY, MSR2(0,0), "IA32_VMX_CR0_FIXED1",
			"Capability Reporting Register of CR0 Bits Fixed to 1", {
		/* Additional info available at Appendix A.7,
		 * "VMX-Fixed Bits in CR0" */
		{ BITS_EOT }
	}},
	{0x488, MSRTYPE_RDONLY, MSR2(0,0), "IA32_VMX_CR4_FIXED0",
			"Capability Reporting Register of CR4 Bits Fixed to 0", {
		/* Additional info available at Appendix A.8,
		 * "VMX-Fixed Bits in CR4" */
		{ BITS_EOT }
	}},
	{0x489, MSRTYPE_RDONLY, MSR2(0,0), "IA32_VMX_CR4_FIXED1",
			"Capability Reporting Register of CR4 Bits Fixed to 1", {
		/* Additional info available at Appendix A.8,
		 * "VMX-Fixed Bits in CR4" */
		{ BITS_EOT }
	}},
	{0x48a, MSRTYPE_RDONLY, MSR2(0,0), "IA32_VMX_VMCS_ENUM",
			"Capability Reporting Register of VMCS Field Enumeration", {
		/* Additional info available at Appendix A.9,
		 * "VMCS Enumeration" */
		{ BITS_EOT }
	}},
	{0x48b, MSRTYPE_RDONLY, MSR2(0,0), "IA32_VMX_PROCBASED_CTLS2",
			"Capability Reporting Register of Secondary \
			Processor-based VM-execution Controls", {
		/* Additional info available at Appendix A.3,
		 * "VM-Execution Controls" */
		{ BITS_EOT }
	}},
#endif
	{0x600, MSRTYPE_RDWR, MSR2(0,0), "IA32_DS_AREA", "DS Save Area", {
		/* Additional info available at Section 18.10.4 of Intel 64
		 * and IA-32 Architectures Software Developer's Manual,
		 * "Debug Store (DS) Mechanism".
		 */
		{ 63, 32, RESERVED }, // reserved if not in IA-32e mode
		{ 31, 32, "Linear address of DS buffer management area",
				"R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ MSR_EOT }
};
