/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

#include <arch/early_variables.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <fsp/car.h>
#include <fsp/util.h>
#include <fsp/memmap.h>
#include <program_loading.h>
#include <timestamp.h>

#define ROMSTAGE_RAM_STACK_SIZE 0x5000

/* platform_enter_postcar() determines the stack to use after
 * cache-as-ram is torn down as well as the MTRR settings to use,
 * and continues execution in postcar stage. */
static void platform_enter_postcar(void)
{
	struct postcar_frame pcf;
	size_t alignment;
	uint32_t aligned_ram;

	if (postcar_frame_init(&pcf, ROMSTAGE_RAM_STACK_SIZE))
		die("Unable to initialize postcar frame.\n");
	/* Cache the ROM as WP just below 4GiB. */
	postcar_frame_add_mtrr(&pcf, CACHE_ROM_BASE, CACHE_ROM_SIZE,
			       MTRR_TYPE_WRPROT);

	/* Cache RAM as WB from 0 -> CACHE_TMP_RAMTOP. */
	postcar_frame_add_mtrr(&pcf, 0, CACHE_TMP_RAMTOP, MTRR_TYPE_WRBACK);

	/*
	 *     +-------------------------+  Top of RAM (aligned)
	 *     | System Management Mode  |
	 *     |      code and data      |  Length: CONFIG_TSEG_SIZE
	 *     |         (TSEG)          |
	 *     +-------------------------+  SMM base (aligned)
	 *     |                         |
	 *     | Chipset Reserved Memory |  Length: Multiple of CONFIG_TSEG_SIZE
	 *     |                         |
	 *     +-------------------------+  top_of_ram (aligned)
	 *     |                         |
	 *     |       CBMEM Root        |
	 *     |                         |
	 *     +-------------------------+
	 *     |                         |
	 *     |   FSP Reserved Memory   |
	 *     |                         |
	 *     +-------------------------+
	 *     |                         |
	 *     |  Various CBMEM Entries  |
	 *     |                         |
	 *     +-------------------------+  top_of_stack (8 byte aligned)
	 *     |                         |
	 *     |   stack (CBMEM Entry)   |
	 *     |                         |
	 *     +-------------------------+
	 */

	alignment = mmap_region_granularity();
	aligned_ram = ALIGN_DOWN(romstage_ram_stack_bottom(), alignment);
	postcar_frame_add_mtrr(&pcf, aligned_ram, alignment, MTRR_TYPE_WRBACK);

	if (CONFIG(HAVE_SMI_HANDLER)) {
		void *smm_base;
		size_t smm_size;

		/*
		 * Cache the TSEG region at the top of ram. This region is not
		 * restricted to SMM mode until SMM has been relocated. By
		 * setting the region to cacheable it provides faster access
		 * when relocating the SMM handler as well as using the TSEG
		 * region for other purposes.
		 */
		smm_region(&smm_base, &smm_size);
		postcar_frame_add_mtrr(&pcf, (uintptr_t)smm_base, alignment,
				       MTRR_TYPE_WRBACK);
	}

	run_postcar_phase(&pcf);
}

/* This is the romstage C entry for platforms without
   CONFIG_C_ENVIRONMENT_BOOTBLOCK */
asmlinkage void cache_as_ram_main(struct cache_as_ram_params *car_params)
{
	int i;
	const int num_guards = 4;
	const u32 stack_guard = 0xdeadbeef;
	u32 *stack_base;
	u32 size;

	/* Size of unallocated CAR. */
	size = _car_region_end - _car_relocatable_data_end;
	size = ALIGN_DOWN(size, 16);

	stack_base = (u32 *)(_car_region_end - size);

	for (i = 0; i < num_guards; i++)
		stack_base[i] = stack_guard;

	/* Initialize timestamp book keeping only once. */
	timestamp_init(car_params->tsc);

	/* Call into pre-console init code then initialize console. */
	car_soc_pre_console_init();
	car_mainboard_pre_console_init();
	console_init();

	printk(BIOS_DEBUG, "FSP TempRamInit successful\n");

	printk(BIOS_SPEW, "bist: 0x%08x\n", car_params->bist);
	printk(BIOS_SPEW, "tsc: 0x%016llx\n", car_params->tsc);

	display_mtrrs();

	if (car_params->bootloader_car_start != CONFIG_DCACHE_RAM_BASE
	    || car_params->bootloader_car_end != (CONFIG_DCACHE_RAM_BASE
						  + CONFIG_DCACHE_RAM_SIZE)) {
		printk(BIOS_INFO, "CAR mismatch: %08x--%08x vs %08lx--%08lx\n",
		       CONFIG_DCACHE_RAM_BASE,
		       CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE,
		       (long)car_params->bootloader_car_start,
		       (long)car_params->bootloader_car_end);
	}

	car_soc_post_console_init();
	car_mainboard_post_console_init();

	cache_as_ram_stage_main(car_params->fih);

	/* Check the stack. */
	for (i = 0; i < num_guards; i++) {
		if (stack_base[i] == stack_guard)
			continue;
		printk(BIOS_DEBUG, "Smashed stack detected in romstage!\n");
	}

	/* we don't return here */
	platform_enter_postcar();
}

/* This is the romstage C entry for platforms with
   CONFIG_C_ENVIRONMENT_BOOTBLOCK */
asmlinkage void romstage_c_entry(void)
{
	/* Need to locate the current FSP_INFO_HEADER. The cache-as-ram
	 * is still enabled. We can directly access work buffer here. */
	FSP_INFO_HEADER *fih;
	struct prog fsp = PROG_INIT(PROG_REFCODE, "fsp.bin");

	console_init();

	if (prog_locate(&fsp)) {
		fih = NULL;
		printk(BIOS_ERR, "Unable to locate %s\n", prog_name(&fsp));
	} else {
		/* This leaks a mapping which this code assumes is benign as
		 * the flash is memory mapped CPU's address space. */
		fih = find_fsp((uintptr_t)rdev_mmap_full(prog_rdev(&fsp)));
	}

	cache_as_ram_stage_main(fih);

	/* we don't return here */
	platform_enter_postcar();
}

void __weak car_mainboard_pre_console_init(void)
{
}

void __weak car_soc_pre_console_init(void)
{
}

void __weak car_mainboard_post_console_init(void)
{
}

void __weak car_soc_post_console_init(void)
{
}
