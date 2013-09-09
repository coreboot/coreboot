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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <stdint.h>
#include <string.h>
#include <cbmem.h>
#include <console/console.h>
#include <arch/cpu.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/stack.h>
#include <lib.h>
#include <timestamp.h>
#include <arch/io.h>
#include <arch/stages.h>
#include <device/pci_def.h>
#include <cpu/x86/lapic.h>
#include <cbfs.h>
#include <romstage_handoff.h>
#include <reset.h>
#if CONFIG_CHROMEOS
#include <vendorcode/google/chromeos/chromeos.h>
#endif
#if CONFIG_EC_GOOGLE_CHROMEEC
#include <ec/google/chromeec/ec.h>
#endif
#include "haswell.h"
#include "northbridge/intel/haswell/haswell.h"
#include "northbridge/intel/haswell/raminit.h"
#include "southbridge/intel/lynxpoint/pch.h"
#include "southbridge/intel/lynxpoint/me.h"


static inline void reset_system(void)
{
	hard_reset();
	while (1) {
		hlt();
	}
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

/* Romstage needs quite a bit of stack for decompressing images since the lzma
 * lib keeps its state on the stack during romstage. */
#define ROMSTAGE_RAM_STACK_SIZE 0x5000
static unsigned long choose_top_of_stack(void)
{
	unsigned long stack_top;
#if CONFIG_DYNAMIC_CBMEM
	/* cbmem_add() does a find() before add(). */
	stack_top = (unsigned long)cbmem_add(CBMEM_ID_ROMSTAGE_RAM_STACK,
	                                     ROMSTAGE_RAM_STACK_SIZE);
	stack_top += ROMSTAGE_RAM_STACK_SIZE;
#else
	stack_top = ROMSTAGE_STACK;
#endif
	return stack_top;
}

/* setup_romstage_stack_after_car() determines the stack to use after
 * cache-as-ram is torn down as well as the MTRR settings to use. */
static void *setup_romstage_stack_after_car(void)
{
	unsigned long top_of_stack;
	int num_mtrrs;
	u32 *slot;
	u32 mtrr_mask_upper;
	u32 top_of_ram;

	/* Top of stack needs to be aligned to a 4-byte boundary. */
	top_of_stack = choose_top_of_stack() & ~3;
	slot = (void *)top_of_stack;
	num_mtrrs = 0;

	/* The upper bits of the MTRR mask need to set according to the number
	 * of physical address bits. */
	mtrr_mask_upper = (1 << ((cpuid_eax(0x80000008) & 0xff) - 32)) - 1;

	/* The order for each MTTR is value then base with upper 32-bits of
	 * each value coming before the lower 32-bits. The reasoning for
	 * this ordering is to create a stack layout like the following:
	 *   +0: Number of MTRRs
	 *   +4: MTTR base 0 31:0
	 *   +8: MTTR base 0 63:32
	 *  +12: MTTR mask 0 31:0
	 *  +16: MTTR mask 0 63:32
	 *  +20: MTTR base 1 31:0
	 *  +24: MTTR base 1 63:32
	 *  +28: MTTR mask 1 31:0
	 *  +32: MTTR mask 1 63:32
	 */

	/* Cache the ROM as WP just below 4GiB. */
	slot = stack_push(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push(slot, ~(CONFIG_ROM_SIZE - 1) | MTRRphysMaskValid);
	slot = stack_push(slot, 0); /* upper base */
	slot = stack_push(slot, ~(CONFIG_ROM_SIZE - 1) | MTRR_TYPE_WRPROT);
	num_mtrrs++;

	/* Cache RAM as WB from 0 -> CONFIG_RAMTOP. */
	slot = stack_push(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push(slot, ~(CONFIG_RAMTOP - 1) | MTRRphysMaskValid);
	slot = stack_push(slot, 0); /* upper base */
	slot = stack_push(slot, 0 | MTRR_TYPE_WRBACK);
	num_mtrrs++;

	top_of_ram = get_top_of_ram();
	/* Cache 8MiB below the top of ram. On haswell systems the top of
	 * ram under 4GiB is the start of the TSEG region. It is required to
	 * be 8MiB aligned. Set this area as cacheable so it can be used later
	 * for ramstage before setting up the entire RAM as cacheable. */
	slot = stack_push(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push(slot, ~((8 << 20) - 1) | MTRRphysMaskValid);
	slot = stack_push(slot, 0); /* upper base */
	slot = stack_push(slot, (top_of_ram - (8 << 20)) | MTRR_TYPE_WRBACK);
	num_mtrrs++;

	/* Cache 8MiB at the top of ram. Top of ram on haswell systems
	 * is where the TSEG region resides. However, it is not restricted
	 * to SMM mode until SMM has been relocated. By setting the region
	 * to cacheable it provides faster access when relocating the SMM
	 * handler as well as using the TSEG region for other purposes. */
	slot = stack_push(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push(slot, ~((8 << 20) - 1) | MTRRphysMaskValid);
	slot = stack_push(slot, 0); /* upper base */
	slot = stack_push(slot, top_of_ram | MTRR_TYPE_WRBACK);
	num_mtrrs++;

	/* Save the number of MTTRs to setup. Return the stack location
	 * pointing to the number of MTRRs. */
	slot = stack_push(slot, num_mtrrs);

	return slot;
}

void * asmlinkage romstage_main(unsigned long bist)
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
	struct romstage_handoff *handoff;

	timestamp_init(get_initial_timestamp());
	timestamp_add_now(TS_START_ROMSTAGE);

	if (params->bist == 0)
		enable_lapic();

	wake_from_s3 = early_pch_init(params->gpio_map, params->rcba_config);

#if CONFIG_EC_GOOGLE_CHROMEEC
	/* Ensure the EC is in the right mode for recovery */
	google_chromeec_early_init();
#endif

	/* Halt if there was a built in self test failure */
	report_bist_failure(params->bist);

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	haswell_early_initialization(HASWELL_MOBILE);
	printk(BIOS_DEBUG, "Back from haswell_early_initialization()\n");

	if (wake_from_s3) {
#if CONFIG_HAVE_ACPI_RESUME
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
	#if CONFIG_HAVE_ACPI_RESUME
		/* Failed S3 resume, reset to come up cleanly */
		reset_system();
	#endif
	}

	handoff = romstage_handoff_find_or_add();
	if (handoff != NULL)
		handoff->s3_resume = wake_from_s3;
	else
		printk(BIOS_DEBUG, "Romstage handoff structure not added!\n");

	post_code(0x3f);
#if CONFIG_CHROMEOS
	init_chromeos(boot_mode);
#endif
	timestamp_add_now(TS_END_ROMSTAGE);
}

static inline void prepare_for_resume(struct romstage_handoff *handoff)
{
/* Only need to save memory when ramstage isn't relocatable. */
#if !CONFIG_RELOCATABLE_RAMSTAGE
#if CONFIG_HAVE_ACPI_RESUME
	/* Back up the OS-controlled memory where ramstage will be loaded. */
	if (handoff != NULL && handoff->s3_resume) {
		void *src = (void *)CONFIG_RAMBASE;
		void *dest = cbmem_find(CBMEM_ID_RESUME);
		if (dest != NULL)
			memcpy(dest, src, HIGH_MEMORY_SAVE);
	}
#endif
#endif
}

void romstage_after_car(void)
{
	struct romstage_handoff *handoff;

	handoff = romstage_handoff_find_or_add();

	prepare_for_resume(handoff);

#if CONFIG_VBOOT_VERIFY_FIRMWARE
	vboot_verify_firmware(handoff);
#endif

	/* Load the ramstage. */
	copy_and_run();
}


#if CONFIG_RELOCATABLE_RAMSTAGE
void cache_loaded_ramstage(struct romstage_handoff *handoff,
                           const struct cbmem_entry *ramstage,
                           void *entry_point)
{
	struct ramstage_cache *cache;
	uint32_t total_size;
	uint32_t ramstage_size;
	void *ramstage_base;

	ramstage_size = cbmem_entry_size(ramstage);
	ramstage_base = cbmem_entry_start(ramstage);

	/* The ramstage cache lives in the TSEG region at RESERVED_SMM_OFFSET.
	 * The top of ram is defined to be the TSEG base address. */
	cache = (void *)(get_top_of_ram() + RESERVED_SMM_OFFSET);
	total_size = sizeof(*cache) + ramstage_size;
	if (total_size > RESERVED_SMM_SIZE) {
		printk(BIOS_DEBUG, "0x%08x > RESERVED_SMM_SIZE (0x%08x)\n",
		       total_size, RESERVED_SMM_SIZE);
		/* Nuke whatever may be there now just in case. */
		cache->magic = ~RAMSTAGE_CACHE_MAGIC;
		return;
	}

	cache->magic = RAMSTAGE_CACHE_MAGIC;
	cache->entry_point = (uint32_t)entry_point;
	cache->load_address = (uint32_t)ramstage_base;
	cache->size = ramstage_size;

	printk(BIOS_DEBUG, "Saving ramstage to SMM space cache.\n");

	/* Copy over the program. */
	memcpy(&cache->program[0], ramstage_base, ramstage_size);

	if (handoff == NULL)
		return;

	handoff->ramstage_entry_point = (uint32_t)entry_point;
}

void *load_cached_ramstage(struct romstage_handoff *handoff,
                           const struct cbmem_entry *ramstage)
{
	struct ramstage_cache *cache;

	/* The ramstage cache lives in the TSEG region at RESERVED_SMM_OFFSET.
	 * The top of ram is defined to be the TSEG base address. */
	cache = (void *)(get_top_of_ram() + RESERVED_SMM_OFFSET);

	if (cache->magic != RAMSTAGE_CACHE_MAGIC) {
		printk(BIOS_DEBUG, "Invalid ramstage cache found.\n");
		#if CONFIG_RESET_ON_INVALID_RAMSTAGE_CACHE
		reset_system();
		#endif
		return NULL;
	}

	printk(BIOS_DEBUG, "Loading ramstage from SMM space cache.\n");

	memcpy((void *)cache->load_address, &cache->program[0], cache->size);

	return (void *)cache->entry_point;
}
#endif
