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
#include <arch/cache.h>
#include <arch/hlt.h>
#include <arch/stages.h>
#include <cbfs.h>
#include <console/console.h>

#include "stages.c"

static void armv7_invalidate_caches(void)
{
	uint32_t clidr;
	int level;

	/* Invalidate branch predictor */
	bpiall();

	/* Iterate thru each cache identified in CLIDR and invalidate */
	clidr = read_clidr();
	for (level = 0; level < 7; level++) {
		unsigned int ctype = (clidr >> (level * 3)) & 0x7;
		uint32_t csselr;

		switch(ctype) {
		case 0x0:
			/* no cache */
			break;
		case 0x1:
			/* icache only */
			csselr = (level << 1) | 1;
			write_csselr(csselr);
			icache_invalidate_all();
			break;
		case 0x2:
		case 0x4:
			/* dcache only or unified cache */
			dcache_invalidate_all();
			break;
		case 0x3:
			/* separate icache and dcache */
			csselr = (level << 1) | 1;
			write_csselr(csselr);
			icache_invalidate_all();

			csselr = level < 1;
			write_csselr(csselr);
			dcache_invalidate_all();
			break;
		default:
			/* reserved */
			break;
		}
	}

	/* Invalidate TLB */
	/* FIXME: ARMv7 Architecture Ref. Manual claims that the distinction
	 * instruction vs. data TLBs is deprecated in ARMv7. But that doesn't
	 * really seem true for Cortex-A15? */
	tlb_invalidate_all();
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
	uint32_t sctlr;

	/* Globally disable MMU, caches, and branch prediction (these should
	 * be disabled by default on reset) */
	sctlr = read_sctlr();
	sctlr &= ~(SCTLR_M | SCTLR_C | SCTLR_Z | SCTLR_I);
	write_sctlr(sctlr);

	armv7_invalidate_caches();

	/*
	 * Re-enable caches and branch prediction. MMU will be set up later.
	 * Note: If booting from USB, we need to disable branch prediction
	 * before copying from USB into RAM (FIXME: why?)
	 */
	sctlr = read_sctlr();
	sctlr |= SCTLR_C | SCTLR_Z | SCTLR_I;
	write_sctlr(sctlr);

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
