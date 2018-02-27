/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 ChromeOS Authors
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

#include <stdint.h>
#include <string.h>
#include <cbfs.h>
#include <console/console.h>
#include <arch/cpu.h>
#include <cpu/cpu.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <halt.h>
#include <lib.h>
#include <timestamp.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <cpu/x86/lapic.h>
#include <cbmem.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <reset.h>
#include <vendorcode/google/chromeos/chromeos.h>
#if IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC)
#include <ec/google/chromeec/ec.h>
#endif
#include "haswell.h"
#include "northbridge/intel/haswell/haswell.h"
#include "northbridge/intel/haswell/raminit.h"
#include "southbridge/intel/lynxpoint/pch.h"
#include "southbridge/intel/lynxpoint/me.h"
#include <security/tpm/tspi.h>

static inline void reset_system(void)
{
	hard_reset();
	halt();
}

/* The cache-as-ram assembly file calls romstage_main() after setting up
 * cache-as-ram.  romstage_main() will then call the mainboards's
 * mainboard_romstage_entry() function. That function then calls
 * romstage_common() below. The reason for the back and forth is to provide
 * common entry point from cache-as-ram while still allowing for code sharing.
 * Because we can't use global variables the stack is used for allocations --
 * thus the need to call back and forth. */


static inline u32 *stack_push(u32 *stack, u32 value)
{
	stack = &stack[-1];
	*stack = value;
	return stack;
}

/* setup_romstage_stack_after_car() determines the stack to use after
 * cache-as-ram is torn down as well as the MTRR settings to use. */
static void *setup_romstage_stack_after_car(void)
{
	int num_mtrrs;
	u32 *slot;
	u32 mtrr_mask_upper;
	u32 top_of_ram;

	/* Top of stack needs to be aligned to a 4-byte boundary. */
	slot = (void *)romstage_ram_stack_top();
	num_mtrrs = 0;

	/* The upper bits of the MTRR mask need to set according to the number
	 * of physical address bits. */
	mtrr_mask_upper = (1 << (cpu_phys_address_size() - 32)) - 1;

	/* The order for each MTRR is value then base with upper 32-bits of
	 * each value coming before the lower 32-bits. The reasoning for
	 * this ordering is to create a stack layout like the following:
	 *   +0: Number of MTRRs
	 *   +4: MTRR base 0 31:0
	 *   +8: MTRR base 0 63:32
	 *  +12: MTRR mask 0 31:0
	 *  +16: MTRR mask 0 63:32
	 *  +20: MTRR base 1 31:0
	 *  +24: MTRR base 1 63:32
	 *  +28: MTRR mask 1 31:0
	 *  +32: MTRR mask 1 63:32
	 */

	/* Cache the ROM as WP just below 4GiB. */
	slot = stack_push(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push(slot, ~(CACHE_ROM_SIZE - 1) | MTRR_PHYS_MASK_VALID);
	slot = stack_push(slot, 0); /* upper base */
	slot = stack_push(slot, ~(CACHE_ROM_SIZE - 1) | MTRR_TYPE_WRPROT);
	num_mtrrs++;

	/* Cache RAM as WB from 0 -> CACHE_TMP_RAMTOP. */
	slot = stack_push(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push(slot, ~(CACHE_TMP_RAMTOP - 1) | MTRR_PHYS_MASK_VALID);
	slot = stack_push(slot, 0); /* upper base */
	slot = stack_push(slot, 0 | MTRR_TYPE_WRBACK);
	num_mtrrs++;

	top_of_ram = (uint32_t)cbmem_top();
	/* Cache 8MiB below the top of RAM. On haswell systems the top of
	 * RAM under 4GiB is the start of the TSEG region. It is required to
	 * be 8MiB aligned. Set this area as cacheable so it can be used later
	 * for ramstage before setting up the entire RAM as cacheable. */
	slot = stack_push(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push(slot, ~((8 << 20) - 1) | MTRR_PHYS_MASK_VALID);
	slot = stack_push(slot, 0); /* upper base */
	slot = stack_push(slot, (top_of_ram - (8 << 20)) | MTRR_TYPE_WRBACK);
	num_mtrrs++;

	/* Cache 8MiB at the top of RAM. Top of RAM on haswell systems
	 * is where the TSEG region resides. However, it is not restricted
	 * to SMM mode until SMM has been relocated. By setting the region
	 * to cacheable it provides faster access when relocating the SMM
	 * handler as well as using the TSEG region for other purposes. */
	slot = stack_push(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push(slot, ~((8 << 20) - 1) | MTRR_PHYS_MASK_VALID);
	slot = stack_push(slot, 0); /* upper base */
	slot = stack_push(slot, top_of_ram | MTRR_TYPE_WRBACK);
	num_mtrrs++;

	/* Save the number of MTRRs to setup. Return the stack location
	 * pointing to the number of MTRRs. */
	slot = stack_push(slot, num_mtrrs);

	return slot;
}

asmlinkage void *romstage_main(unsigned long bist)
{
	int i;
	void *romstage_stack_after_car;
	const int num_guards = 4;
	const u32 stack_guard = 0xdeadbeef;
	u32 *stack_base = (void *)(CONFIG_DCACHE_RAM_BASE +
				   CONFIG_DCACHE_RAM_SIZE -
				   CONFIG_DCACHE_RAM_ROMSTAGE_STACK_SIZE);

	printk(BIOS_DEBUG, "Setting up stack guards.\n");
	for (i = 0; i < num_guards; i++)
		stack_base[i] = stack_guard;

	mainboard_romstage_entry(bist);

	/* Check the stack. */
	for (i = 0; i < num_guards; i++) {
		if (stack_base[i] == stack_guard)
			continue;
		printk(BIOS_DEBUG, "Smashed stack detected in romstage!\n");
	}

	/* Get the stack to use after cache-as-ram is torn down. */
	romstage_stack_after_car = setup_romstage_stack_after_car();

	return romstage_stack_after_car;
}

void romstage_common(const struct romstage_params *params)
{
	int boot_mode;
	int wake_from_s3;

	timestamp_init(get_initial_timestamp());
	timestamp_add_now(TS_START_ROMSTAGE);

	if (params->bist == 0)
		enable_lapic();

	wake_from_s3 = early_pch_init(params->gpio_map, params->rcba_config);

	/* Halt if there was a built in self test failure */
	report_bist_failure(params->bist);

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	haswell_early_initialization(HASWELL_MOBILE);
	printk(BIOS_DEBUG, "Back from haswell_early_initialization()\n");

	if (wake_from_s3) {
#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
		printk(BIOS_DEBUG, "Resume from S3 detected.\n");
#else
		printk(BIOS_DEBUG, "Resume from S3 detected, but disabled.\n");
		wake_from_s3 = 0;
#endif
	}

	/* There are hard coded assumptions of 2 meaning s3 wake. Normalize
	 * the users of the 2 literal here based off wake_from_s3. */
	boot_mode = wake_from_s3 ? 2 : 0;

	/* Prepare USB controller early in S3 resume */
	if (wake_from_s3)
		enable_usb_bar();

	post_code(0x3a);
	params->pei_data->boot_mode = boot_mode;

	timestamp_add_now(TS_BEFORE_INITRAM);

	report_platform_info();

	if (params->copy_spd != NULL)
		params->copy_spd(params->pei_data);

	sdram_initialize(params->pei_data);

	timestamp_add_now(TS_AFTER_INITRAM);

	post_code(0x3b);

	intel_early_me_status();

	quick_ram_check();
	post_code(0x3e);

	if (!wake_from_s3) {
		cbmem_initialize_empty();
		/* Save data returned from MRC on non-S3 resumes. */
		save_mrc_data(params->pei_data);
	} else if (cbmem_initialize()) {
	#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
		/* Failed S3 resume, reset to come up cleanly */
		reset_system();
	#endif
	}

	setup_sdram_meminfo(params->pei_data);

	romstage_handoff_init(wake_from_s3);

	post_code(0x3f);
	if (IS_ENABLED(CONFIG_TPM1) || IS_ENABLED(CONFIG_TPM2))
		tpm_setup(wake_from_s3);
}

asmlinkage void romstage_after_car(void)
{
	/* Load the ramstage. */
	run_ramstage();
}
