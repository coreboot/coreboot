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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/car.h>
#include <cpu/amd/mtrr.h>
#include <cpu/x86/cache.h>
#include <cbmem.h>
#include <string.h>
#include <halt.h>
#include "s3_resume.h"

static void *backup_resume(void)
{
	void *resume_backup_memory;

	printk(BIOS_DEBUG, "Find resume memory location\n");

	if (cbmem_recovery(1)) {
		printk(BIOS_EMERG, "Unable to recover CBMEM\n");
		halt();
	}

	resume_backup_memory = cbmem_find(CBMEM_ID_RESUME);
	if (resume_backup_memory == NULL) {
		printk(BIOS_EMERG, "No storage for low-memory backup\n");
		halt();
	}

	return resume_backup_memory;
}

static void move_stack_high_mem(void)
{
	void *high_stack = cbmem_find(CBMEM_ID_ROMSTAGE_RAM_STACK);

	/* TODO: Make the switch with empty stack instead. */
	memcpy(high_stack, (void *)BSP_STACK_BASE_ADDR, HIGH_ROMSTAGE_STACK_SIZE);

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

static void set_resume_cache(void)
{
	msr_t msr;

	/* disable fixed mtrr for now,  it will be enabled by mtrr restore */
	msr = rdmsr(SYSCFG_MSR);
	msr.lo &= ~(SYSCFG_MSR_MtrrFixDramEn | SYSCFG_MSR_MtrrFixDramModEn);
	wrmsr(SYSCFG_MSR, msr);

	/* Enable caching for 0 - coreboot ram using variable mtrr */
	msr.lo = 0 | MTRR_TYPE_WRBACK;
	msr.hi = 0;
	wrmsr(MTRR_PHYS_BASE(0), msr);
	msr.lo = ~(CONFIG_RAMTOP - 1) | MTRR_PHYS_MASK_VALID;
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
	void *resume_backup_memory = backup_resume();

	post_code(0x62);
	printk(BIOS_DEBUG, "Move CAR stack.\n");
	move_stack_high_mem();

	post_code(0x63);
	disable_cache_as_ram();
	printk(BIOS_DEBUG, "CAR disabled.\n");
	set_resume_cache();

	/*
	 * Copy the system memory that is in the ramstage area to the
	 * reserved area.
	 */
	if (resume_backup_memory)
		memcpy(resume_backup_memory, (void *)(CONFIG_RAMBASE), HIGH_MEMORY_SAVE);

	printk(BIOS_DEBUG, "System memory saved. OK to load ramstage.\n");
}
