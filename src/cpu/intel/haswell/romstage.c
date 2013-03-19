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
#include <arch/romcc_io.h>
#include <device/pci_def.h>
#include <cpu/x86/lapic.h>
#include <cbmem.h>
#include <cbfs.h>
#include <romstage_handoff.h>
#if CONFIG_CHROMEOS
#include <vendorcode/google/chromeos/chromeos.h>
#endif
#include "haswell.h"
#include "northbridge/intel/haswell/haswell.h"
#include "northbridge/intel/haswell/raminit.h"
#include "southbridge/intel/lynxpoint/pch.h"
#include "southbridge/intel/lynxpoint/me.h"


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

static unsigned long choose_top_of_stack(void)
{
	unsigned long stack_top;
#if CONFIG_RELOCATABLE_RAMSTAGE
	stack_top = (unsigned long)cbmem_add(CBMEM_ID_RESUME_SCRATCH,
	                                     CONFIG_HIGH_SCRATCH_MEMORY_SIZE);
	stack_top += CONFIG_HIGH_SCRATCH_MEMORY_SIZE;
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

void * __attribute__((regparm(0))) romstage_main(unsigned long bist)
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

#if CONFIG_CONSOLE_CBMEM
	/* Keep this the last thing this function does. */
	cbmemc_reinit();
#endif

	return romstage_stack_after_car;
}

void romstage_common(const struct romstage_params *params)
{
	int boot_mode;
	int wake_from_s3;
	int cbmem_was_initted;
	struct romstage_handoff *handoff;

#if CONFIG_COLLECT_TIMESTAMPS
	tsc_t start_romstage_time;
	tsc_t before_dram_time;
	tsc_t after_dram_time;
	tsc_t base_time = {
		.lo = pci_read_config32(PCI_DEV(0, 0x00, 0), 0xdc),
		.hi = pci_read_config32(PCI_DEV(0, 0x1f, 2), 0xd0)
	};
#endif

#if CONFIG_COLLECT_TIMESTAMPS
	start_romstage_time = rdtsc();
#endif

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
#if CONFIG_COLLECT_TIMESTAMPS
	before_dram_time = rdtsc();
#endif

	report_platform_info();

	sdram_initialize(params->pei_data);

#if CONFIG_COLLECT_TIMESTAMPS
	after_dram_time = rdtsc();
#endif
	post_code(0x3b);

	intel_early_me_status();

	quick_ram_check();
	post_code(0x3e);

#if CONFIG_EARLY_CBMEM_INIT
	cbmem_was_initted = !cbmem_initialize();
#else
	cbmem_was_initted = cbmem_reinit((uint64_t) (get_top_of_ram()
						     - HIGH_MEMORY_SIZE));
#endif

	/* Save data returned from MRC on non-S3 resumes. */
	if (!wake_from_s3)
		save_mrc_data(params->pei_data);

#if CONFIG_HAVE_ACPI_RESUME
	if (wake_from_s3 && !cbmem_was_initted) {
		/* Failed S3 resume, reset to come up cleanly */
		outb(0x6, 0xcf9);
		while (1) {
			hlt();
		}
	}
#endif

	handoff = romstage_handoff_find_or_add();
	if (handoff != NULL)
		handoff->s3_resume = wake_from_s3;
	else
		printk(BIOS_DEBUG, "Romstage handoff structure not added!\n");

	post_code(0x3f);
#if CONFIG_CHROMEOS
	init_chromeos(boot_mode);
#endif
#if CONFIG_COLLECT_TIMESTAMPS
	timestamp_init(base_time);
	timestamp_add(TS_START_ROMSTAGE, start_romstage_time );
	timestamp_add(TS_BEFORE_INITRAM, before_dram_time );
	timestamp_add(TS_AFTER_INITRAM, after_dram_time );
	timestamp_add_now(TS_END_ROMSTAGE);
#endif
}

static inline void prepare_for_resume(void)
{
/* Only need to save memory when ramstage isn't relocatable. */
#if !CONFIG_RELOCATABLE_RAMSTAGE
#if CONFIG_HAVE_ACPI_RESUME
	struct romstage_handoff *handoff = romstage_handoff_find_or_add();

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
	prepare_for_resume();
	/* Load the ramstage. */
	copy_and_run(0);
}


#if CONFIG_RELOCATABLE_RAMSTAGE
void cache_loaded_ramstage(struct romstage_handoff *handoff,
                           void *ramstage_base, uint32_t ramstage_size,
                           void *entry_point)
{
	struct ramstage_cache *cache;
	uint32_t total_size;

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

	/* Do not update reserve region if the handoff structure is not
	 * available. Perhaps the ramstage will fix things up for the resume
	 * path. */
	if (handoff == NULL)
		return;

	/* Update entry and reserve region. */
	handoff->reserve_base = (uint32_t)ramstage_base;
	handoff->reserve_size = ramstage_size;
	handoff->ramstage_entry_point = (uint32_t)entry_point;
}

void *load_cached_ramstage(struct romstage_handoff *handoff)
{
	struct ramstage_cache *cache;

	/* The ramstage cache lives in the TSEG region at RESERVED_SMM_OFFSET.
	 * The top of ram is defined to be the TSEG base address. */
	cache = (void *)(get_top_of_ram() + RESERVED_SMM_OFFSET);

	if (cache->magic != RAMSTAGE_CACHE_MAGIC) {
		printk(BIOS_DEBUG, "Invalid ramstage cache found.\n");
		return NULL;
	}

	printk(BIOS_DEBUG, "Loading ramstage from SMM space cache.\n");

	memcpy((void *)cache->load_address, &cache->program[0], cache->size);

	return (void *)cache->entry_point;
}
#endif
