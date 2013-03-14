/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Google Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <bootblock_common.h>
#include <arch/hlt.h>
#include <arch/stages.h>
#include <cbfs.h>
#include <console/console.h>
#include <system.h>

#include "stages.c"

/*
 * Set/clear program flow prediction and return the previous state.
 */
static int config_branch_prediction(int set_cr_z)
{
	unsigned int cr;

	/* System Control Register: 11th bit Z Branch prediction enable */
	cr = get_cr();
	set_cr(set_cr_z ? cr | CR_Z : cr & ~CR_Z);

	return cr & CR_Z;
}

/* Cache and MMU init */
static void armv7_invalidate_caches(void)
{
	unsigned int cr;

	/*
	 * From Cortex-A Series Programmers guide (Example 15-3):
	 * 1. Disable MMU
	 * 2. Disable L1 caches (example disables i-cache then d-cache)
	 * 3. Invalidate L1 caches (same order)
	 * 4. Invalidate TLB
	 *
	 * L2 cache setup will be done by SoC-specific code. MMU setup
	 * needs to be done after DRAM init in board-specific code.
	 */

	/* Disable MMU */
	cr = get_cr();
	cr &= ~CR_M;
	set_cr(cr);

	/* Disable L1 caches */
	icache_disable();
	dcache_disable();

	/* Invalidate caches */
	invalidate_icache_all();
	invalidate_dcache_all();

	/* Invalidate TLB */
	v7_inval_tlb();
}

static int boot_cpu(void)
{
	/*
	 * FIXME: This is a stub for now. All non-boot CPUs should be
	 * waiting for an interrupt. We could move the chunk of assembly
	 * which puts them to sleep in here...
	 */
	return 1;
}

void main(void)
{
	const char *stage_name = "fallback/romstage";
	void *entry;

	armv7_invalidate_caches();

	/*
	 * Branch prediction enable.
	 * Note: If booting from USB, we need to disable branch prediction
	 * before copying from USB into RAM (FIXME: why?)
	 */
	config_branch_prediction(1);

	if (boot_cpu()) {
		bootblock_cpu_init();
		bootblock_mainboard_init();
	}

	console_init();
	printk(BIOS_INFO, "hello from bootblock\n");
	printk(BIOS_INFO, "bootblock main(): loading romstage\n");
	entry = cbfs_load_stage(CBFS_DEFAULT_MEDIA, stage_name);

	printk(BIOS_INFO, "bootblock main(): jumping to romstage\n");
	if (entry) stage_exit(entry);
	hlt();
}
