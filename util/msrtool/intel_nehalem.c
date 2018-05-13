/*
 * This file is part of msrtool.
 *
 * Copyright (C) 2012 Anton Kochkov <anton.kochkov@gmail.com>
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

int intel_nehalem_probe(const struct targetdef *target, const struct cpuid_t *id) {
	return ((VENDOR_INTEL == id->vendor) &&
		(0x6 == id->family) && (
		(0x1a == id->model) ||
		(0x1e == id->model) ||
		(0x1f == id->model) ||
		(0x2e == id->model) ||
		(0x25 == id->model) ||	/* westmere */
		(0x2c == id->model) ||	/* westmere */
		(0x2f == id->model)	/* westmere */
		));
}

const struct msrdef intel_nehalem_msrs[] = {
	{0x17, MSRTYPE_RDWR, MSR2(0,0), "IA32_PLATFORM_ID Register",
			"Model Specific Platform ID", {
		{ 63, 11, RESERVED },
		{ 52, 3, RESERVED },
		{ 49, 37, RESERVED },
		{ 12, 5, "Maximum Qualified Ratio:", "The maximum allowed bus ratio",
				PRESENT_DEC, {
			{ BITVAL_EOT }
		}},
		{ 7, 8, RESERVED },
		{ BITS_EOT }
	}},
	/* FIXME: This MSR not documented for Nehalem */
	{0xcd, MSRTYPE_RDONLY, MSR2(0,0), "MSR_FSB_FREQ", "Scaleable Bus Speed", {
	/* This field indicates the intended scaleable bus clock speed */
		{ BITS_EOT }
	}},
	{0xce, MSRTYPE_RDONLY, MSR2(0,0), "MSR_PLATFORM_INFO", "", {
		{ 63, 16, RESERVED },
		{ 47, 8, "Maximum Efficiency Ratio", "R/O", PRESENT_DEC, {
			{ BITVAL_EOT }
		}},
		{ 39, 10, RESERVED },
		{ 29, 1, "Programmable TDC-TDP Limit for Turbo Mode", "R/O", PRESENT_DEC, {
			{ MSR1(0), "TDC and TDP Limits for Turbo Mode are not programmable" },
			{ MSR1(1), "TDC and TDP Limits for Turbo Mode are programmable" },
			{ BITVAL_EOT }
		}},
		{ 28, 1, "Programmable Ratio Limit for Turbo Mode", "R/O", PRESENT_DEC, {
			{ MSR1(0), "Programmable Ratio Limit for Turbo Mode is disabled" },
			{ MSR1(1), "Programmable Ratio Limit for Turbo Mode is enabled" },
			{ BITVAL_EOT }
		}},
		{ 27, 12, RESERVED },
		{ 15, 8, "Maximum Non-Turbo Ratio", "R/O", PRESENT_DEC, {
			/* The is ratio of the frequency that invariant TSC runs at. The invariant
			 * TSC requency can be computed by multipying this ratio by 133.33 Mhz
			 */
			{ BITVAL_EOT }
		}},
		{ 7, 8, RESERVED },
		{ BITS_EOT }
	}},
	{0x11e, MSRTYPE_RDWR, MSR2(0,0), "MSR_BBL_CR_CTL3", "", {
		{ BITS_EOT }
	}},
	/* FIXME: There is already two 0x1ad MSRs for Nehalem in the
	 * Intel 64 and IA-32 Architectures Software Developer's Manual
	 * Volume 3C 34-91. But from decimal value of this register,
	 * we can conclude, that it was just typo, and this register
	 * have address 0x1ac.
	 */
	{0x1ac, MSRTYPE_RDWR, MSR2(0,0), "MSR_TURBO_POWER_CURRENT_LIMIT", "", {
		{ 63, 32, RESERVED },
		{ 31, 1, "TDC Limit Override Enable", "R/W", PRESENT_BIN, {
			{ MSR1(0), "TDC Limit Override is not active" },
			{ MSR1(1), "TDC Limit Override is active" },
			{ BITVAL_EOT }
		}},
		{ 30, 15, "TDC Limit", "R/W", PRESENT_HEX, {
			/* TDC Limit in 1/8 Amp granularity */
			{ BITVAL_EOT }
		}},
		{ 15, 1, "TDP Limit Override Enable", "R/W", PRESENT_BIN, {
			{ MSR1(0), "TDP Limit Override is not active" },
			{ MSR1(1), "TDP Limit Override is active" },
			{ BITVAL_EOT }
		}},
		{ 14, 15, "TDP Limit", "R/W", PRESENT_HEX, {
			/* TDP Limit in 1/8 Watt granularity */
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x1ad, MSRTYPE_RDWR, MSR2(0,0), "MSR_TURBO_RATIO_LIMIT",
			"Maximum Ratio Limit of Turbo Mode", {
	// "RO" if MSR_PLATFORM_INFO.[28] = 0
	// "RW" if MSR_PLATFORM_INFO.[23] = 1
		{ 63, 32, RESERVED },
		{ 31, 8, "Maximum Ratio Limit for 4C", "R/O", PRESENT_HEX, {
			// Maximum Turbo Ratio Limit of 4 core active
			{ BITVAL_EOT }
		}},
		{ 23, 8, "Maximum Ratio Limit for 3C", "R/O", PRESENT_HEX, {
			// Maximum Turbo Ratio Limit of 3 core active
			{ BITVAL_EOT }
		}},
		{ 15, 8, "Maximum Ratio Limit for 2C", "R/O", PRESENT_HEX, {
			// Maximum Turbo Ratio Limit of 2 core active
			{ BITVAL_EOT }
		}},
		{ 7, 8, "Maximum Ratio Limit for 1C", "R/O", PRESENT_HEX, {
			// Maximum Turbo Ratio Limit of 1 core active
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x280, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC0_CTL2", "", {
		{ 63, 33, RESERVED },
		{ 30, 1, "CMCI_EN", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 29, 15, RESERVED },
		{ 14, 15, "Corrected error count threshold", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x281, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC1_CTL2", "", {
		{ 63, 33, RESERVED },
		{ 30, 1, "CMCI_EN", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 29, 15, RESERVED },
		{ 14, 15, "Corrected error count threshold", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x286, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC6_CTL2", "", {
		{ 63, 33, RESERVED },
		{ 30, 1, "CMCI_EN", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 29, 15, RESERVED },
		{ 14, 15, "Corrected error count threshold", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x287, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC7_CTL2", "", {
		{ 63, 33, RESERVED },
		{ 30, 1, "CMCI_EN", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 29, 15, RESERVED },
		{ 14, 15, "Corrected error count threshold", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x288, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC8_CTL2", "", {
		{ 63, 33, RESERVED },
		{ 30, 1, "CMCI_EN", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 29, 15, RESERVED },
		{ 14, 15, "Corrected error count threshold", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x3f8, MSRTYPE_RDONLY, MSR2(0,0), "MSR_PKG_C3_RESIDENCY", "", {
		{ 63, 64, "Package C3 Residency Counter", "R/O", PRESENT_DEC, {
		/* Value since last reset that this package is in C3 states.
		 * Count at the same frequency as the TSC.
		 */
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x3f9, MSRTYPE_RDONLY, MSR2(0,0), "MSR_PKG_C6_RESIDENCY", "", {
		{ BITS_EOT }
	}},
	{0x3fa, MSRTYPE_RDONLY, MSR2(0,0), "MSR_PKG_C7_RESIDENCY", "", {
		{ BITS_EOT }
	}},
	{0x418, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC6_CTL", "", {
		{ BITS_EOT }
	}},
	{0x419, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC6_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x41a, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC6_ADDR", "", {
		{ BITS_EOT }
	}},
	{0x41b, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC6_MISC", "", {
		{ BITS_EOT }
	}},
	{0x41c, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC7_CTL", "", {
		{ BITS_EOT }
	}},
	{0x41d, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC7_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x41e, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC7_ADDR", "", {
		{ BITS_EOT }
	}},
	{0x41f, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC7_MISC", "", {
		{ BITS_EOT }
	}},
	{0x420, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC8_CTL", "", {
		{ BITS_EOT }
	}},
	{0x421, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC8_STATUS", "", {
		{ BITS_EOT }
	}},
	{0x422, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC8_ADDR", "", {
		{ BITS_EOT }
	}},
	{0x423, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC8_MISC", "", {
		{ BITS_EOT }
	}},

/* ==========================================================================
 *                             Per core MSRs
 * ==========================================================================
 */

	{0x0, MSRTYPE_RDWR, MSR2(0,0), "IA32_P5_MC_ADDR", "Pentium Processor\
			Machine-Check Exception Address", {
		{ BITS_EOT }
	}},
	{0x1, MSRTYPE_RDWR, MSR2(0,0), "IA32_P5_MC_TYPE", "Pentium Processor\
			Machine-Check Exception Type", {
		{ BITS_EOT }
	}},
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
	{0x34, MSRTYPE_RDONLY, MSR2(0,0), "MSR_SMI_COUNT", "SMI Counter register", {
		{ 63, 32, RESERVED },
		{ 31, 32, "SMI Count", "R/O", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
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
			 * VMX in conjuction with SMX to support Intel
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
	{0x79, MSRTYPE_RDWR, MSR2(0,0), "IA32_BIOS_UPDT_TRIG",
			"BIOS Update Trigger Register (W)", {
		{ BITS_EOT }
	}},
	{0x8b, MSRTYPE_RDWR, MSR2(0,0), "IA32_BIOS_SIGN_ID",
			"BIOS Update Signature ID (RO)", {
		{ BITS_EOT }
	}},
	{0xa0, MSRTYPE_RDWR, MSR2(0,0), "MSR_SMRR_PHYS_BASE", "", {
		{ BITS_EOT }
	}},
	{0xa1, MSRTYPE_RDWR, MSR2(0,0), "MSR_SMRR_PHYS_MASK", "", {
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
	{0xc3, MSRTYPE_RDWR, MSR2(0,0), "IA32_PMC2",
			"Performance counter register", {
		{ BITS_EOT }
	}},
	{0xc4, MSRTYPE_RDWR, MSR2(0,0), "IA32_PMC3",
			"Performance counter register", {
		{ BITS_EOT }
	}},
	{0xe2, MSRTYPE_RDWR, MSR2(0,0), "MSR_PKG_CST_CONFIG_CONTROL",
			"C-State Configuration Control", {
		{ 63, 37, RESERVED },
		{ 26, 1, "C1 state auto demotion", "R/W", PRESENT_DEC, {
			{ MSR1(0), "Demotion of C3/C6/C7 requests to C1 is disabled" },
			{ MSR1(1), "Demotion of C3/C6/C7 requests to C1 is enabled" },
			{ BITVAL_EOT }
		}},
		{ 25, 1, "C3 state auto demotion", "R/W", PRESENT_DEC, {
			{ MSR1(0), "Demotion of C6/C7 requests to C3 is disabled" },
			{ MSR1(1), "Demotion of C6/C7 requests to C3 is enabled" },
			{ BITVAL_EOT }
		}},
		{ 24, 1, "Interrupt filtering enabled/disabled", "R/W", PRESENT_DEC, {
			{ MSR1(0), "All CPU cores in deep C-State will wake for an \
				event message" },
			{ MSR1(1), "CPU in deep C-State will wake only when the event \
				message is destined for that core" },
			{ BITVAL_EOT }
		}},
		{ 23, 8, RESERVED },
		{ 15, 1, "CFG Lock", "R/WO", PRESENT_DEC, {
			{ MSR1(0), "[15:0] bits of MSR_PKG_CST_CONFIG_CONTROL(0xe2) \
				are writeable" },
			{ MSR1(1), "[15:0] bits of MSR_PKG_CST_CONFIG_CONTROL(0xe2) \
				are locked until reset" },
			{ BITVAL_EOT }
		}},
		{ 14, 4, RESERVED },
		{ 10, 1, "I/O MWAIT Redirection", "R/W", PRESENT_DEC, {
			{ MSR1(0), "I/O MWAIT Redirection disabled" },
			{ MSR1(1), "CPU will map IO_read instructions sent to \
				IO register specified by MSR_PMG_IO_CAPTURE_BASE \
				to MWAIT instructions" },
			{ BITVAL_EOT }
		}},
		{ 9, 7, RESERVED },
		{ 2, 3, "Package C-State limit", "R/W", PRESENT_BIN, {
			/* Specifies the lowest processor specific C-state code name
			 * (consuming the least power) for the package. The default is set
			 * as factory-configured package C-state limit.
			 */
			{ MSR1(0), "C0 (no package C-state support)" },
			{ MSR1(1), "C1 (behavior is the same as C0)" },
			{ MSR1(2), "C3" },
			{ MSR1(3), "C6" },
			{ MSR1(4), "C7" },
			{ MSR1(5), "Reserved" },
			{ MSR1(6), "Reserved" },
			{ MSR1(7), "No package C-state limit" },
			{ BITVAL_EOT }
			/* Note: this field cannot be used to limit
			 * package C-state to C3
			 */
		}},
		{ BITS_EOT }
	}},
	{0xe4, MSRTYPE_RDWR, MSR2(0,0), "MSR_PMG_IO_CAPTURE_BASE",
			"Power Management IO Redirection in C-state", {
		{ 63, 45, RESERVED },
		{ 18, 3, "C-state Range", "R/W", PRESENT_BIN, {
			/* Specifies the encoding value of the maximum C-State code name
			 * to be included when IO read to MWAIT redirection is enabled by
			 * MSR_PMG_CST_CONFIG_CONTROL[bit10].
			 */
			{ MSR1(0), "C3 is the max C-State to include" },
			{ MSR1(1), "C6 is the max C-State to include" },
			{ MSR1(2), "C7 is the max C-State to include" },
			{ BITVAL_EOT }
		}},
		{ 15, 16, "LVL_2 Base Address", "R/W", PRESENT_HEX, {
			/* Specifies the base address visible to software for IO redirection.
			 * If I/O MWAIT Redirection is enabled, reads to this address will be
			 * consumed by the power management logic and decoded to MWAIT
			 * instructions. When IO port address redirection is enabled,
			 * this is the I/O port address reported to the OS/software.
			 */
			{ BITVAL_EOT }
		}},
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
	/* if CPUID.0AH: EAX[15:8] > 0 */
	{0x188, MSRTYPE_RDWR, MSR2(0,0), "IA32_PERFEVTSEL2",
			"Performance Event Select Register 2", {
		{ 63, 32, RESERVED },
		{ 31, 8, "CMASK", "R/W", PRESENT_HEX, {
			/* When CMASK is not zero, the corresponding performance
			 * counter 2 increments each cycle if the event count
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
	{0x189, MSRTYPE_RDWR, MSR2(0,0), "IA32_PERFEVTSEL3",
			"Performance Event Select Register 3", {
		{ 63, 32, RESERVED },
		{ 31, 8, "CMASK", "R/W", PRESENT_HEX, {
			/* When CMASK is not zero, the corresponding performance
			 * counter 3 increments each cycle if the event count
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
		{ 63, 48, RESERVED },
		{ 15, 16, "Current Performance State Value", "R/O", PRESENT_HEX, {
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
	{0x19d, MSRTYPE_RDWR, MSR2(0,0), "MSR_THERM2_CTL", "", {
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
	{0x1a2, MSRTYPE_RDWR, MSR2(0,0), "MSR_TEMPERATURE_TARGET", "", {
		{ 63, 40, RESERVED },
		{ 23, 8, "Temperature Target", "R", PRESENT_DEC, {
			/* The minimum temperature at which PROCHOT# will be
			 * asserted. The value in degree C.
			 */
			{ BITVAL_EOT }
		}},
		{ 15, 16, RESERVED },
		{ BITS_EOT }
	}},
	{0x1a6, MSRTYPE_RDWR, MSR2(0,0), "MSR_OFFCORE_RSP_O",
			"Offcore Response Event Select Register", {
		{ BITS_EOT }
	}},
	{0x1aa, MSRTYPE_RDWR, MSR2(0,0), "MSR_MISC_PWR_MGMT", "", {
		{ 63, 62, RESERVED },
		{ 1, 1, "Energy/Performance Bias Enable", "R/W", PRESENT_BIN, {
			/* This bit status is also reflected
			 * by CPUID.(EAX=06h):ECX[3]
			 */
			{ MSR1(0), "IA32_ENERGY_PERF_BIAS (0x1b0) is invisible \
				for Ring 0 software" },
			{ MSR1(1), "IA32_ENERGY_PERF_BIAS (0x1b0) accessible \
				by Ring 0 software" },
			{ BITVAL_EOT }
		}},
		{ 0, 1, "EIST Hardware Coordination Disable", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Hardware Coordination of EIST request \
				from processor cores is enabled" },
			{ MSR1(1), "Hardware Coordination of EIST request \
				from processor cores is disabled" },
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x1c8, MSRTYPE_RDWR, MSR2(0,0), "MSR_LBR_SELECT",
		"Last Branch Record Filtering Select Register", {
	/*  "Nehalem support filtering of LBR based on combination of CPL
	 *  and branch type conditions. When LBR filtering is enabled,
	 *  the LBR stack only captures the subset of branches
	 *  that are specified by MSR_LBR_SELECT."
	 *
	 *   -- Section 17.6.2 of Intel 64 and IA-32 Architectures Software
	 *   Developer's Manual, Volume 3
	 */
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
	{0x1f2, MSRTYPE_RDONLY, MSR2(0,0), "IA32_SMRR_PHYS_BASE",
		"SMRR Base Address", {
	/* Base address of SMM memory range.
	 * Writeable only in SMM, so marking it as read-only */
		{ 63, 32, RESERVED },
		{ 31, 20, "SMRR physical Base Address", "R/O", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 11, 4, RESERVED },
		{ 7, 8, "Memory type of the range", "R/O", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x1f3, MSRTYPE_RDONLY, MSR2(0,0), "IA32_SMRR_PHYS_MASK",
		"SMRR Range Mask", {
	/* Range Mask of SMM memory range.
	 * Writeable only in SMM, so marking it as read-only */
		{ 63, 32, RESERVED },
		{ 31, 20, "SMRR address range mask", "R/O", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 11, 1, "Is Valid SMRR range mask", "R/O", PRESENT_BIN, {
			{ MSR1(0), "SMRR range mask is disabled" },
			{ MSR1(1), "SMRR range mask is enabled" },
			{ BITVAL_EOT }
		}},
		{ 10, 11, RESERVED },
		{ BITS_EOT }
	}},
	{0x1f8, MSRTYPE_RDONLY, MSR2(0,0), "IA32_PLATFORM_DCA_CAP",
			"DCA Capability", {
		{ BITS_EOT }
	}},
	{0x1f9, MSRTYPE_RDONLY, MSR2(0,0), "IA32_CPU_DCA_CAP",
		"Support og Prefetch-Hint type", {
	/* If set, CPU supports Prefetch-Hint type.
	 * TODO: As it is undocumented, which bit (or bits)
	 * are needed to "be set", we need collect some
	 * outputs of msrtool to understand possible msr values.
	 */
		{ BITS_EOT }
	}},
	{0x1fa, MSRTYPE_RDWR, MSR2(0,0), "IA32_DCA_0_CAP",
		"DCA type 0 Status and Control register", {
	/* This register defined as introduced only
	 * in 06_2EH Nehalem model (latest), so be careful!
	 */
		{ 31, 5, RESERVED },
		{ 26, 1, "HW_BLOCK", "R/O", PRESENT_BIN, {
		/* Hardware block of DCA */
			{ MSR1(0), "DCA is not blocked by HW" },
			{ MSR1(1), "DCA is blocked by HW (e.g. CR0.CD=1)" },
			{ BITVAL_EOT }
		}},
		{ 25, 1, RESERVED },
		{ 24, 1, "SW_BLOCK", "R/W", PRESENT_BIN, {
		/* Software block of DCA */
			{ MSR1(0), "DCA is not blocked by SW" },
			{ MSR1(1), "DCA is blocked by SW" },
			{ BITVAL_EOT }
		}},
		{ 23, 7, RESERVED },
		{ 16, 4, "DCA_RELAY", "R/W", PRESENT_HEX, {
		/* Writes will update the register
		 * but have no HW side-effect */
			{ BITVAL_EOT }
		}},
		{ 12, 2, RESERVED },
		{ 10, 4, "DCA_QUEUE_SIZE", "R/O", PRESENT_DEC, {
			{ BITVAL_EOT }
		}},
		{ 6, 4, "DCA_TYPE", "R/O", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 2, 2, "TRANSACTION", "R/O", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 0, 1, "DCA_ACTIVE", "R/O", PRESENT_BIN, {
		/* Set by HW when DCA is fuse-enabled and
		 * no defeauteres are set */
			{ MSR1(0), "DCA inactive" },
			{ MSR1(1), "DCA inactive" },
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x1fc, MSRTYPE_RDWR, MSR2(0,0), "MSR_POWER_CTL",
			"Power Control Register", {
		{ 63, 62, RESERVED },
		/* Whole package bit */
		{ 1, 1, "C1E Enable", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Nothing" },
			{ MSR1(1), "CPU switch to the Minimum Enhaced Intel \
				SpeedStep Technology operating point when all \
				execution cores enter MWAIT (C1)" },
			{ BITVAL_EOT }
		}},
		{ 0, 1, RESERVED },
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
	/* if IA32_MTRR_CAP[7:0] > 8 */
	{0x210, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYS_BASE8", "", {
		{ BITS_EOT }
	}},
	/* if IA32_MTRR_CAP[7:0] > 8 */
	{0x211, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYS_MASK8", "", {
		{ BITS_EOT }
	}},
	/* if IA32_MTRR_CAP[7:0] > 9 */
	{0x212, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYS_BASE9", "", {
		{ BITS_EOT }
	}},
	/* if IA32_MTRR_CAP[7:0] > 9 */
	{0x213, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_PHYS_MASK9", "", {
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
	{0x282, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC3_CTL2", "", {
		{ 63, 33, RESERVED },
		{ 30, 1, "CMCI_EN", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 29, 15, RESERVED },
		{ 14, 15, "Corrected error count threshold", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x283, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC3_CTL2", "", {
		{ 63, 33, RESERVED },
		{ 30, 1, "CMCI_EN", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 29, 15, RESERVED },
		{ 14, 15, "Corrected error count threshold", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x284, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC4_CTL2", "", {
		{ 63, 33, RESERVED },
		{ 30, 1, "CMCI_EN", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 29, 15, RESERVED },
		{ 14, 15, "Corrected error count threshold", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x285, MSRTYPE_RDWR, MSR2(0,0), "IA32_MC5_CTL2", "", {
		{ 63, 33, RESERVED },
		{ 30, 1, "CMCI_EN", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 29, 15, RESERVED },
		{ 14, 15, "Corrected error count threshold", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x2ff, MSRTYPE_RDWR, MSR2(0,0), "IA32_MTRR_DEF_TYPE",
			"Default Memory Types", {
		{ 63, 52, RESERVED },
		{ 11, 1, "MTRR Enable", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 10, 1, "Fixed Range MTRR Enable", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 9, 7, RESERVED },
		{ 2, 3, "Default Memory Type", "R/W", PRESENT_HEX, {
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
				conditions occuring in the logical processor which programmed the MSR" },
			{ MSR1(1), "Counting the associated event conditions \
				occuring across all logical processors sharing a processor core" },
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
				conditions occuring in the logical processor which programmed the MSR" },
			{ MSR1(1), "Counting the associated event conditions \
				occuring across all logical processors sharing a processor core" },
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
				conditions occuring in the logical processor which programmed the MSR" },
			{ MSR1(1), "Counting the associated event conditions \
				occuring across all logical processors sharing a processor core" },
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
	{0x3f6, MSRTYPE_RDWR, MSR2(0,0), "MSR_PEBS_LD_LAT", "", {
	/* See Section 18.6.1.2 of Intel's manual
	 * for additional information.
	 */
		{ 63, 28, RESERVED },
		{ 35, 20, RESERVED },
		{ 15, 16, "Minimum threshold latency value of tagged \
				load operation that will be counted", "R/W", PRESENT_DEC, {
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
	/* Undocumented PECI control register */
	{0x5a0, MSRTYPE_RDWR, MSR2(0,0), "IA32_PECI_CTL",
			"PECI Control Register", {
		{ BITS_EOT }
	}},
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

	/*                   16 registers - first
	 *                registers in sixteen pairs
	 *              of last branch record registers
	 *              on the last branch record stack
	 */
	{0x680, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_0_FROM_IP", "R/W", {
		{ BITS_EOT }
	}},
	{0x681, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_1_FROM_IP", "R/W", {
		{ BITS_EOT }
	}},
	{0x682, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_2_FROM_IP", "R/W", {
		{ BITS_EOT }
	}},
	{0x683, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_3_FROM_IP", "R/W", {
		{ BITS_EOT }
	}},
	{0x684, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_4_FROM_IP", "R/W", {
		{ BITS_EOT }
	}},
	{0x685, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_5_FROM_IP", "R/W", {
		{ BITS_EOT }
	}},
	{0x686, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_6_FROM_IP", "R/W", {
		{ BITS_EOT }
	}},
	{0x687, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_7_FROM_IP", "R/W", {
		{ BITS_EOT }
	}},
	{0x688, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_8_FROM_IP", "R/W", {
		{ BITS_EOT }
	}},
	{0x689, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_9_FROM_IP", "R/W", {
		{ BITS_EOT }
	}},
	{0x68a, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_10_FROM_IP", "R/W", {
		{ BITS_EOT }
	}},
	{0x68b, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_11_FROM_IP", "R/W", {
		{ BITS_EOT }
	}},
	{0x68c, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_12_FROM_IP", "R/W", {
		{ BITS_EOT }
	}},
	{0x68d, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_13_FROM_IP", "R/W", {
		{ BITS_EOT }
	}},
	{0x68e, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_14_FROM_IP", "R/W", {
		{ BITS_EOT }
	}},
	{0x68f, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_15_FROM_IP", "R/W", {
		{ BITS_EOT }
	}},

	/*                    16 registers - second
	 *				    registers in sixteen pairs
	 *              of last branch record registers
	 *              on the last branch record stack
	 */
	{0x6c0, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_0_TO_LIP", "R/W", {
		{ BITS_EOT }
	}},
	{0x6c1, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_1_TO_LIP", "R/W", {
		{ BITS_EOT }
	}},
	{0x6c2, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_2_TO_LIP", "R/W", {
		{ BITS_EOT }
	}},
	{0x6c3, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_3_TO_LIP", "R/W", {
		{ BITS_EOT }
	}},
	{0x6c4, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_4_TO_LIP", "R/W", {
		{ BITS_EOT }
	}},
	{0x6c5, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_5_TO_LIP", "R/W", {
		{ BITS_EOT }
	}},
	{0x6c6, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_6_TO_LIP", "R/W", {
		{ BITS_EOT }
	}},
	{0x6c7, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_7_TO_LIP", "R/W", {
		{ BITS_EOT }
	}},
	{0x6c8, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_8_TO_LIP", "R/W", {
		{ BITS_EOT }
	}},
	{0x6c9, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_9_TO_LIP", "R/W", {
		{ BITS_EOT }
	}},
	{0x6ca, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_10_TO_LIP", "R/W", {
		{ BITS_EOT }
	}},
	{0x6cb, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_11_TO_LIP", "R/W", {
		{ BITS_EOT }
	}},
	{0x6cc, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_12_TO_LIP", "R/W", {
		{ BITS_EOT }
	}},
	{0x6cd, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_13_TO_LIP", "R/W", {
		{ BITS_EOT }
	}},
	{0x6ce, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_14_TO_LIP", "R/W", {
		{ BITS_EOT }
	}},
	{0x6cf, MSRTYPE_RDWR, MSR2(0,0), "MSR_LASTBRANCH_15_TO_LIP", "R/W", {
		{ BITS_EOT }
	}},

	/* x2APIC registers - see Intel 64 Architecture x2APIC Specification */

	{0x802, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_APICID",
			"x2APIC ID register", {
		{ BITS_EOT }
	}},
	{0x803, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_VERSION",
	/* Same version between extended and legacy modes.
	 * Bit 24 is available only to an x2APIC unit. */
			"x2APIC Version register", {
		{ 31, 7, RESERVED },
		{ 24, 1, "Directed EOI Support", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 23, 8, "Max LVT Entry", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 15, 8, RESERVED },
		{ 7, 8, "Vector", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x808, MSRTYPE_RDWR, MSR2(0,0), "IA32_X2APIC_TPR",
			"x2APIC Task Priority register", {
		{ 31, 24, RESERVED },
		{ 7, 8, "TPR", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x80a, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_PPR",
			"x2APIC Processor Priority register", {
		{ BITS_EOT }
	}},
	{0x80b, MSRTYPE_WRONLY, MSR2(0,0), "IA32_X2APIC_EOI",
	/* 0 is the only valid value to write. GP fault
	 * on non-zero write. */
			"x2APIC EOI register", {
		{ BITS_EOT }
	}},
	{0x80d, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_LDR",
	/* Read Only in x2APIC mode, Read-Write in xAPIC mode. */
			"x2APIC Logical Destination register", {
		{ BITS_EOT }
	}},
	{0x80f, MSRTYPE_RDWR, MSR2(0,0), "IA32_X2APIC_SIVR",
			"x2APIC Spurious Interrupt Vector register", {
		{ 31, 19, RESERVED },
		{ 12, 1, "EOI Broadcast Disable", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 11, 3, RESERVED },
		{ 8, 1, "APIC Software Enable/Disable", "R/W", PRESENT_BIN, {
			{ MSR1(0), "APIC Disabled" },
			{ MSR1(1), "APIC Enabled" },
			{ BITVAL_EOT }
		}},
		{ 7, 8, "Spurious Vector", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x810, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_ISR_0",
			"x2APIC In-Service register bits [31:0]", {
		{ BITS_EOT }
	}},
	{0x811, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_ISR_1",
			"x2APIC In-Service register bits [63:32]", {
		{ BITS_EOT }
	}},
	{0x812, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_ISR_2",
			"x2APIC In-Service register bits [95:64]", {
		{ BITS_EOT }
	}},
	{0x813, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_ISR_3",
			"x2APIC In-Service register bits [127:96]", {
		{ BITS_EOT }
	}},
	{0x814, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_ISR_4",
			"x2APIC In-Service register bits [159:128]", {
		{ BITS_EOT }
	}},
	{0x815, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_ISR_5",
			"x2APIC In-Service register bits [191:160]", {
		{ BITS_EOT }
	}},
	{0x816, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_ISR_6",
			"x2APIC In-Service register bits [223:192]", {
		{ BITS_EOT }
	}},
	{0x817, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_ISR_7",
			"x2APIC In-Service register bits [255:224]", {
		{ BITS_EOT }
	}},
	{0x818, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_TMR0",
			"x2APIC Trigger Mode register bits [31:0]", {
		{ BITS_EOT }
	}},
	{0x819, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_TMR1",
			"x2APIC Trigger Mode register bits [63:32]", {
		{ BITS_EOT }
	}},
	{0x81a, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_TMR2",
			"x2APIC Trigger Mode register bits [95:64]", {
		{ BITS_EOT }
	}},
	{0x81b, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_TMR3",
			"x2APIC Trigger Mode register bits [127:96]", {
		{ BITS_EOT }
	}},
	{0x81c, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_TMR4",
			"x2APIC Trigger Mode register bits [159:128]", {
		{ BITS_EOT }
	}},
	{0x81d, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_TMR5",
			"x2APIC Trigger Mode register bits [191:160]", {
		{ BITS_EOT }
	}},
	{0x81e, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_TMR6",
			"x2APIC Trigger Mode register bits [223:192]", {
		{ BITS_EOT }
	}},
	{0x81f, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_TMR7",
			"x2APIC Trigger Mode register bits [255:224]", {
		{ BITS_EOT }
	}},
	{0x820, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_IRR0",
			"x2APIC Interrupt Request register bits [31:0]", {
		{ BITS_EOT }
	}},
	{0x821, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_IRR1",
			"x2APIC Trigger Mode register bits [63:32]", {
		{ BITS_EOT }
	}},
	{0x822, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_IRR2",
			"x2APIC Trigger Mode register bits [95:64]", {
		{ BITS_EOT }
	}},
	{0x823, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_IRR3",
			"x2APIC Trigger Mode register bits [127:96]", {
		{ BITS_EOT }
	}},
	{0x824, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_IRR4",
			"x2APIC Trigger Mode register bits [159:128]", {
		{ BITS_EOT }
	}},
	{0x825, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_IRR5",
			"x2APIC Trigger Mode register bits [191:160]", {
		{ BITS_EOT }
	}},
	{0x826, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_IRR6",
			"x2APIC Trigger Mode register bits [223:192]", {
		{ BITS_EOT }
	}},
	{0x827, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_IRR7",
			"x2APIC Trigger Mode register bits [255:224]", {
		{ BITS_EOT }
	}},
	{0x828, MSRTYPE_RDWR, MSR2(0,0), "IA32_X2APIC_ESR",
	/* GP fault on non-zero writes. */
			"x2APIC Error Status register", {
		{ 31, 24, RESERVED },
		{ 7, 1, "Illegal Register Address", "R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 6, 1, "Received Illegal Vector", "R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 5, 1, "Send Illegal Vector", "R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 4, 1, "Redirectible IPI", "R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 3, 4, RESERVED },
		{ BITS_EOT }
	}},
	{0x82f, MSRTYPE_RDWR, MSR2(0,0), "IA32_X2APIC_LVT_CMCI",
			"x2APIC LVT Corrected Machine Check Interrupt register", {
		{ BITS_EOT }
	}},
	{0x830, MSRTYPE_RDWR, MSR2(0,0), "IA32_X2APIC_ICR",
			"x2APIC Interrupt Command register", {
		{ 63, 32, "Destination field", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 31, 12, RESERVED },
		{ 19, 2, "Destination Shorthand", "R/W", PRESENT_BIN, {
			{ MSR1(0), "No Shorthand" },
			{ MSR1(1), "Self" },
			{ MSR1(2), "All including Self" },
			{ MSR1(3), "All excluding Self" },
			{ BITVAL_EOT }
		}},
		{ 17, 2, RESERVED },
		{ 15, 1, "Trigger Mode", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Edge" },
			{ MSR1(1), "Level" },
			{ BITVAL_EOT }
		}},
		{ 14, 1, "Level", "R/W", PRESENT_BIN, {
			{ MSR1(0), "De-assert" },
			{ MSR1(1), "Assert" },
			{ BITVAL_EOT }
		}},
		{ 13, 2, RESERVED },
		{ 11, 1, "Destination Mode", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Physical" },
			{ MSR1(1), "Logical" },
			{ BITVAL_EOT }
		}},
		{ 10, 3, "Delivery Mode", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Fixed" },
			{ MSR1(1), "Reserved" },
			{ MSR1(2), "SMI" },
			{ MSR1(3), "Reserved" },
			{ MSR1(4), "NMI" },
			{ MSR1(5), "INIT" },
			{ MSR1(6), "Start Up" },
			{ MSR1(7), "Reserved" },
			{ BITVAL_EOT }
		}},
		{ 7, 8, "Vector", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{0x832, MSRTYPE_RDWR, MSR2(0,0), "IA32_X2APIC_LVT_TIMER",
			"x2APIC LVT Timer Interrupt register", {
		{ BITS_EOT }
	}},
	{0x833, MSRTYPE_RDWR, MSR2(0,0), "IA32_X2APIC_LVT_THERMAL",
			"x2APIC LVT Thermal Sensor Interrupt register", {
		{ BITS_EOT }
	}},
	{0x834, MSRTYPE_RDWR, MSR2(0,0), "IA32_X2APIC_LVT_PMI",
			"x2APIC LVT Performance Monitor register", {
		{ BITS_EOT }
	}},
	{0x835, MSRTYPE_RDWR, MSR2(0,0), "IA32_X2APIC_LVT_LINT0",
			"x2APIC LVT LINT0 register", {
		{ BITS_EOT }
	}},
	{0x836, MSRTYPE_RDWR, MSR2(0,0), "IA32_X2APIC_LVT_LINT1",
			"x2APIC LVT LINT1 register", {
		{ BITS_EOT }
	}},
	{0x837, MSRTYPE_RDWR, MSR2(0,0), "IA32_X2APIC_LVT_ERROR",
			"x2APIC LVT Error register", {
		{ BITS_EOT }
	}},
	{0x838, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_INIT_COUNT",
			"x2APIC Initial Count register", {
		{ BITS_EOT }
	}},
	{0x839, MSRTYPE_RDONLY, MSR2(0,0), "IA32_X2APIC_CUR_COUNT",
			"x2APIC Current Count register", {
		{ BITS_EOT }
	}},
	{0x83e, MSRTYPE_RDWR, MSR2(0,0), "IA32_X2APIC_DIV_CONF",
			"x2APIC Divide Configuration register", {
		{ BITS_EOT }
	}},
	{0x83f, MSRTYPE_WRONLY, MSR2(0,0), "IA32_X2APIC_SELF_IPI",
			"x2APIC Self IPI register", {
	/* Only in x2APIC mode. */
		{ 31, 24, RESERVED },
		{ 7, 8, "Vector", "R/W", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
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
	/* if CPUID.80000001H: EDX[29] = 1 or CPUID.80000001H: EDX[27] = 1 */
	{0xc0000080, MSRTYPE_RDWR, MSR2(0,0), "IA32_EFER",
			"Extended Feature Enables", {
		{ 63, 52, RESERVED },
		{ 11, 1, "Execute Disable Bit", "R/O", PRESENT_BIN, {
			{ MSR1(0), "NX bit disabled" },
			{ MSR1(1), "NX bit enabled" },
			{ BITVAL_EOT }
		}},
		{ 10, 1, "IA-32e Mode", "R/O", PRESENT_BIN, {
			{ MSR1(0), "IA-32e Mode Inactive" },
			{ MSR1(1), "IA-32e Mode Active" },
			{ BITVAL_EOT }
		}},
		{ 9, 1, RESERVED },
		{ 8, 1, "IA-32e Mode Control", "R/W", PRESENT_BIN, {
		/* Enable IA-32e Mode operation */
			{ BITVAL_EOT }
		}},
		{ 7, 7, RESERVED },
		{ 0, 1, "SYSCALL Control", "R/W", PRESENT_BIN, {
		/* Enable SYSCALL/SYSRET instructions
		 * in 64-bit mode. */
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	/* if CPUID.80000001H: EDX[29] = 1 */
	{0xc0000081, MSRTYPE_RDWR, MSR2(0,0), "IA32_STAR",
			"System Call Target Address", {
		{ BITS_EOT }
	}},
	/* if CPUID.80000001H: EDX[29] = 1 */
	{0xc0000082, MSRTYPE_RDWR, MSR2(0,0), "IA32_LSTAR",
			"IA32e Mode System Call Target Address", {
		{ BITS_EOT }
	}},
	/* if CPUID.80000001H: EDX[29] = 1 */
	{0xc0000084, MSRTYPE_RDWR, MSR2(0,0), "IA32_FMASK",
			"System Call Flag Mask", {
		{ BITS_EOT }
	}},
	/* if CPUID.80000001H: EDX[29] = 1 */
	{0xc0000100, MSRTYPE_RDWR, MSR2(0,0), "IA32_FS_BASE",
			"Map of BASE Address of FS", {
		{ BITS_EOT }
	}},
	/* if CPUID.80000001H: EDX[29] = 1 */
	{0xc0000101, MSRTYPE_RDWR, MSR2(0,0), "IA32_GS_BASE",
			"Map of BASE Address of GS", {
		{ BITS_EOT }
	}},
	/* if CPUID.80000001H: EDX[29] = 1 */
	{0xc0000102, MSRTYPE_RDWR, MSR2(0,0), "IA32_KERNEL_GS_BASE",
			"Swap Target of BASE Address of GS", {
		{ BITS_EOT }
	}},
	/* if CPUID.80000001H: EDX[27] = 1 */
	{0xc0000103, MSRTYPE_RDWR, MSR2(0,0), "IA32_TSC_AUX",
			"AUXILIARY TSC Signature", {
		{ BITS_EOT }
	}},
	{ MSR_EOT }
};
