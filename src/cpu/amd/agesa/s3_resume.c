/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/acpi.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/car.h>
#include <cpu/amd/mtrr.h>
#include <cpu/x86/cache.h>
#include <cbmem.h>
#include <program_loading.h>
#include <string.h>
#include <halt.h>
#include "s3_resume.h"
#include <northbridge/amd/agesa/agesa_helper.h>

static void move_stack_high_mem(void)
{
	uintptr_t high_stack = romstage_ram_stack_base(HIGH_ROMSTAGE_STACK_SIZE,
		ROMSTAGE_STACK_CBMEM);
	if (!high_stack)
		halt();

	/* TODO: Make the switch with empty stack instead. */
	memcpy((void*)high_stack, (void *)BSP_STACK_BASE_ADDR, HIGH_ROMSTAGE_STACK_SIZE);

	/* TODO: We only switch stack on BSP. */
#ifdef __x86_64__
	__asm__
	    volatile ("add	%0, %%rsp; add %0, %%rbp; invd"::"g"
		      (high_stack - BSP_STACK_BASE_ADDR)
		      :);
#else
	__asm__
	    volatile ("add	%0, %%esp; add %0, %%ebp; invd"::"g"
		      (high_stack - BSP_STACK_BASE_ADDR)
		      :);
#endif
}

void set_resume_cache(void)
{
	msr_t msr;

	/* disable fixed mtrr for now,  it will be enabled by mtrr restore */
	msr = rdmsr(SYSCFG_MSR);
	msr.lo &= ~(SYSCFG_MSR_MtrrFixDramEn | SYSCFG_MSR_MtrrFixDramModEn);
	wrmsr(SYSCFG_MSR, msr);

	/* Enable cached access to RAM in the range 0M to CACHE_TMP_RAMTOP */
	msr.lo = 0 | MTRR_TYPE_WRBACK;
	msr.hi = 0;
	wrmsr(MTRR_PHYS_BASE(0), msr);
	msr.lo = ~(CACHE_TMP_RAMTOP - 1) | MTRR_PHYS_MASK_VALID;
	msr.hi = (1 << (CONFIG_CPU_ADDR_BITS - 32)) - 1;
	wrmsr(MTRR_PHYS_MASK(0), msr);

	/* Set the default memory type and disable fixed and enable variable MTRRs */
	msr.hi = 0;
	msr.lo = (1 << 11);
	wrmsr(MTRR_DEF_TYPE_MSR, msr);

	enable_cache();
}

void prepare_for_resume(void)
{
	if (cbmem_recovery(1)) {
		printk(BIOS_EMERG, "Unable to recover CBMEM\n");
		halt();
	}

	post_code(0x62);
	printk(BIOS_DEBUG, "Move CAR stack.\n");
	move_stack_high_mem();

	post_code(0x63);
	disable_cache_as_ram();
	printk(BIOS_DEBUG, "CAR disabled.\n");
	set_resume_cache();

}
