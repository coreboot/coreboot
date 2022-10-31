/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <amdblocks/cpu.h>
#include <cpu/amd/msr.h>
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <stdint.h>

asmlinkage void bootblock_resume_entry(void);

void write_resume_eip(void)
{
	msr_t s3_resume_entry = {
		.hi = (uint64_t)(uintptr_t)bootblock_resume_entry >> 32,
		.lo = (uintptr_t)bootblock_resume_entry & 0xffffffff,
	};

	/*
	 * Writing to the EIP register can only be done once, otherwise a fault is triggered.
	 * When this register is written, it will trigger the microcode to stash the CPU state
	 * (crX , mtrrs, registers, etc) into the CC6 save area. On resume, the state will be
	 * restored and execution will continue at the EIP.
	 */
	if (!acpi_is_wakeup_s3())
		wrmsr(S3_RESUME_EIP_MSR, s3_resume_entry);
}
