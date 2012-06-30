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
	return ((0x6 == id->family) && (0x17 == id->model));
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
	{0x11e, MSRTYPE_RDWR, MSR2(0,0), "MSR_BBL_CR_CTL3", "", {
		{ 63, 40, RESERVED },
		{ 23, 1, "L2 Present", "R/O", PRESENT_BIN, {
			{ MSR1(0), "L2 Present" },
			{ MSR1(1), "L2 Not Present" },
			{ BITVAL_EOT }
		}},
		{ 22, 14, RESERVED },
		{ 8, 1, "L2 Enabled", "R/W", PRESENT_BIN, {
			{ MSR1(0), "L2 is disabled" },
			{ MSR1(1), "L2 cache has been initialized" },
			{ BITVAL_EOT }
		}},
		{ 7, 7, RESERVED},
		{ 0, 1, "L2 Hardware Enabled", "R/O", PRESENT_BIN, {
			{ MSR1(0), "L2 is hardware-disabled" },
			{ MSR1(1), "L2 is hardware-enabled" },
			{ BITVAL_EOT }
		}},
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
		{ 10, 1, RESERVED },
		{ 9, 1, RESERVED },
		{ 8, 1, "BSP Flag", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 7, 8, RESERVED },
		{ BITS_EOT }
	}},
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
		{ 7, 4, RESERVED },
		{ 3, 1, "SMRR Enable", "R/WL", PRESENT_BIN, {
			{ MSR1(0), "SMRR_PHYS_BASE and SMRR_PHYS_MASK are invisible in SMM" },
			{ MSR1(1), "SMRR_PHYS_BASE and SMRR_PHYS_MASK accessible from SMM" },
			{ BITVAL_EOT }
		}},
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
			{ MSR1(1), "VMX outside of SMX operation enabled" },
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
		{ 63, 52, RESERVED },
		{ 11, 1, "SMRR Capability Using MSR 0xa0 and 0xa1", "R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 10, 11, RESERVED },
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
	{0x198, MSRTYPE_RDWR, MSR2(0,0), "IA32_PERF_STATUS", "", {
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
		{ 63, 24, RESERVED },
		{ 39, 1, "IP Prefetcher Disable", "R/W", PRESENT_BIN, {
			{ MSR1(0), "IP Prefetcher enabled" },
			{ MSR1(1), "IP Prefetcher disabled" },
			{ BITVAL_EOT }
		}},
		/* Note: [38] bit using for whole package,
		 * while some other bits can be Core or Thread
		 * specific.
		 */
		{ 38, 1, "IDA Disable", "R/W", PRESENT_BIN, {
			/* When set to a 0 on processors that support IDA,
			 * CPUID.06H: EAX[1] reports the processor's
			 * support of turbo mode is enabled.
			 */
			{ MSR1(0), "IDA enabled" },
			/* When set 1 on processors that support Intel Turbo Boost
			 * technology, the turbo mode feature is disabled and
			 * the IDA_Enable feature flag will be clear (CPUID.06H: EAX[1]=0).
			 */
			{ MSR1(1), "IDA disabled" },
			{ BITVAL_EOT }
			/* Note: the power-on default value is used by BIOS to detect
			 * hardware support of turbo mode. If power-on default value is 1,
			 * turbo mode is available in the processor. If power-on default
			 * value is 0, turbo mode not available.
			 */
		}},
		{ 37, 1, "DCU Prefetcher Disable", "R/W", PRESENT_BIN, {
			{ MSR1(0), "DCU L1 data cache prefetcher is enabled" },
			{ MSR1(1), "DCU L1 data cache prefetcher is disabled" },
			{ BITVAL_EOT }
		}},
		{ 36, 2, RESERVED },
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
		{ 21, 1, RESERVED },
		{ 20, 1, "Enhanced Intel SpeedStep Select Lock", "R/W",
				PRESENT_BIN, {
			{ MSR1(0), "Enhanced Intel SpeedStep Select\
				and Enable bits are writeable" },
			{ MSR1(1), "Enhanced Intel SpeedStep Select\
				and Enable bits are locked and R/O" },
			{ BITVAL_EOT }
		}},
		{ 19, 1, "Adjacent Cache Line Prefetch Disable", "R/W",
				PRESENT_BIN, {
			{ MSR1(0), "Fetching cache lines that comprise a cache\
				line pair (128 bytes)" },
			{ MSR1(1), "Fetching cache line that contains data\
				currently required by the processor" },
			{ BITVAL_EOT }
		}},
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
		{ 15, 2, RESERVED },
		{ 13, 1, "TM2 Enable", "R/W", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 12, 1, "Precise Event Based Sampling Unavailable", "R/O",
				PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 11, 1, "Branch Trace Storage Unavailable", "R/O", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 10, 1, "FERR# Multiplexing Enable", "R/W", PRESENT_BIN, {
			{ MSR1(0), "FERR# signaling compatible behaviour" },
			{ MSR1(1), "FERR# asserted by the processor to indicate\
				a pending break event within the processor" },
			{ BITVAL_EOT }
		}},
		{ 9, 1, "Hardware Prefetcher Disable", "R/W", PRESENT_BIN, {
			{ MSR1(0), "Hardware prefetcher is enabled" },
			{ MSR1(1), "Hardware prefetcher is disabled" },
			{ BITVAL_EOT }
		}},
		{ 8, 1, RESERVED },
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


