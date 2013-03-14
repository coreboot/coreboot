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

/* FIXME: gross hack to get around src/include/lib.h's romcc limitation. Note:
 * returns 0 in the log2(0) case. */
static unsigned long log2(unsigned long u)
{
	int i = 0;

	while (u >>= 1)
		i++;

	return i;
}

#if 0
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
#endif

static inline uint32_t read_clidr(void)
{
	uint32_t val = 0;
	asm volatile ("mrc p15, 1, %0, c0, c0, 1" : "=r" (val));
	return val;
}

static inline uint32_t read_ccsidr(void)
{
	uint32_t val = 0;
	asm volatile ("mrc p15, 1, %0, c0, c0, 0" : "=r" (val));
	return val;
}

static inline uint32_t read_csselr(void)
{
	uint32_t val = 0;
	asm volatile ("mrc p15, 2, %0, c0, c0, 0" : "=r" (val));
	return val;
}

/* Write to Cache Size Selection Register (CSSELR) */
static inline void write_csselr(uint32_t val)
{
	/*
	 * Bits [3:1] - Cache level + 1 (0b000 = L1, 0b110 = L7, 0b111 is rsvd)
	 * Bit 0 - 0 = data or unified cache, 1 = instruction cache
	 */
	asm volatile ("mcr p15, 2, %0, c0, c0, 0" : : "r" (val));
	isb();	/* ISB to sync the change in case CCSIDR follows */
}

/* Branch predictor invalidate all */
static inline void bpiall(void)
{
	asm volatile ("mcr p15, 0, %0, c7, c5, 6" : : "r" (0));
}

/* Invalidate entire unified TLB */
static inline void tlbiall(void)
{
	asm volatile ("mcr p15, 0, %0, c8, c7, 0" : : "r" (0));
}

/* Instruction cache invalidate all by PoU */
static inline void iciallu(void)
{
	asm volatile ("mcr p15, 0, %0, c7, c5, 0" : : "r" (0));
}

/* Data cache invalidate all by set/way */
static inline void dcisw(uint32_t val)
{
	asm volatile ("mcr p15, 0, %0, c7, c6, 2" : : "r" (val));
}

static inline void dsb(void)
{
	asm volatile ("dsb");
}

/* FIXME: replace isb() in system.h */
static inline void myisb(void)
{
	asm volatile ("isb");
}

static void clear_icache(unsigned int level)
{
	uint32_t ccselr;

	ccselr = (level << 1) | 1;
	write_csselr(ccselr);

	/* icache can be entirely invalidated with one operation.
	 * Note: If branch predictors are architecturally-visible, ICIALLU
	 * also performs a BPIALL operation (B2-1283 in arch manual)
	 */
	iciallu();
	isb();
}

#define bitmask(high, low) ((1UL << (high)) + \
			((1UL << (high)) - 1) - ((1UL << (low)) - 1))

static void clear_dcache(unsigned int level)
{
	uint32_t ccsidr, ccselr;
	unsigned int associativity, num_sets, linesize_bytes;
	unsigned int set, way;

	ccselr = level << 1;
	write_csselr(ccselr);

	/*
	 * dcache must be invalidated by set/way for portability since virtual
	 * memory mapping is system-defined. The number of sets and
	 * associativity is given by CCSIDR. We'll use DCISW to invalidate the
	 * dcache.
	 */
	ccsidr = read_ccsidr();

	/* FIXME: rounding up required here? */
	num_sets = ((ccsidr & bitmask(27, 13)) >> 13) + 1;
	associativity = ((ccsidr & bitmask(12, 3)) >> 3) + 1;
	/* FIXME: do we need to use CTR.DminLine here? */
	linesize_bytes = (1 << ((ccsidr & 0x7) + 2)) * 4;

	/*
	 * Set/way operations require an interesting bit packing. See section
	 * B4-35 in the ARMv7 Architecture Reference Manual:
	 *
	 * A: Log2(associativity)
	 * B: L+S
	 * L: Log2(linesize)
	 * S: Log2(num_sets)
	 *
	 * The bits are packed as follows:
	 *  31  31-A        B B-1    L L-1   4 3   1 0
	 * |---|-------------|--------|-------|-----|-|
	 * |Way|    zeros    |   Set  | zeros |level|0|
	 * |---|-------------|--------|-------|-----|-|
	 */
	for (way = 0; way < associativity; way++) {
		for (set = 0; set < num_sets; set++) {
			uint32_t val = 0;
			val |= way << (32 - log2(associativity));
			val |= set << log2(linesize_bytes);
			val |= level << 1;
			dcisw(val);
		}
	}

	dsb();
}

static void armv7_invalidate_caches(void)
{
	uint32_t clidr;
	int level;

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

	/* Invalidate branch predictor */
	bpiall();

	/*
	 * Iterate thru each cache identified in CLIDR and invalidate.
	 */
	clidr = read_clidr();
	for (level = 0; level < 7; level++) {
		unsigned int ctype = (clidr >> (level * 3)) & 0x7;
		switch(ctype) {
		case 0x0:
			/* no cache */
			break;
		case 0x1:
			/* icache only */
			clear_icache(level);
			break;
		case 0x2:
		case 0x4:
			/* dcache only or unified cache */
			clear_dcache(level);
			break;
		case 0x3:
			/* separate icache and dcache */
			clear_icache(level);
			clear_dcache(level);
			break;
		default:
			/* reserved */
			break;
		}
	}

	/* Invalidate TLB */
	v7_inval_tlb();
	/* FIXME: ARMv7 Architecture Ref. Manual claims that the distinction
	 * instruction vs. data TLBs is deprecated in ARMv7. But that doesn't
	 * really seem true for Cortex-A15? */
//	itlbiall();
//	dtlbiall();
//	tlbiall();
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
	sctlr = get_cr();
	sctlr &= ~(CR_M | CR_C | CR_Z | CR_I);
	set_cr(sctlr);

	armv7_invalidate_caches();

	/*
	 * Re-enable caches and branch prediction. MMU will be set up later.
	 * Note: If booting from USB, we need to disable branch prediction
	 * before copying from USB into RAM (FIXME: why?)
	 */
	sctlr = get_cr();
	sctlr |= CR_C | CR_Z | CR_I;
	set_cr(sctlr);

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
