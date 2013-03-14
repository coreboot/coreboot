/*
 * Copyright (C) 2013 The ChromeOS Authors
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <console/console.h>
#include <cbmem.h>
#include <arch/exception.h>
#include <cpu/samsung/exynos5250/clk.h>
#include <cpu/samsung/exynos5250/power.h>

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
}

#ifndef __mask
# define __mask(high, low) ((1UL << (high)) + \
                            (((1UL << (high)) - 1) - ((1UL << (low)) - 1)))
#endif

static void foo(void)
{
	uint32_t clidr, ccselr, ccsidr;
	int level, assoc, nsets, linesize;

	/* algorithm described by B.2.2.1 in ArmV7 Arch manual */
	clidr = read_clidr();
	printk(BIOS_DEBUG, "%s: clidr: 0x%08x\n", __func__, clidr);
	for (level = 0; level < 7; level++) {
		unsigned int ctype = (clidr >> (level * 3)) & 0x7;

		ccselr = ccsidr = 0;	/* FIXME: paranoia */
		switch(ctype) {
		case 0x0:
			/* no cache */
			printk(BIOS_DEBUG, "L%d has no cache\n", level + 1);
			break;
		case 0x1:
			/* icache only */
			ccselr = (level << 1) | 1;
			write_csselr(ccselr);
			ccsidr = read_ccsidr();
			printk(BIOS_DEBUG, "L%d (icache only): 0x%08x\n", level + 1, ccsidr);
			nsets = ((ccsidr & __mask(27, 13)) >> 13) + 1;
			assoc = ((ccsidr & __mask(12, 3)) >> 3) + 1;
			linesize = 2 << ((ccsidr & 0x7) + 2);
			printk(BIOS_DEBUG, "\tnsets: %d, assoc: %d, linesize: %d\n", nsets, assoc, linesize);
			break;
		case 0x2:
			/* dcache only */
			ccselr = level << 1;
			write_csselr(ccselr);
			ccsidr = read_ccsidr();
			printk(BIOS_DEBUG, "L%d (dcache only): 0x%08x\n", level + 1, ccsidr);
			nsets = ((ccsidr & __mask(27, 13)) >> 13) + 1;
			assoc = ((ccsidr & __mask(12, 3)) >> 3) + 1;
			linesize = (1 << ((ccsidr & 0x7) + 2)) * 4;
			printk(BIOS_DEBUG, "\tnsets: %d, assoc: %d, linesize: %d\n", nsets, assoc, linesize);
			break;
		case 0x3:
			/* separate icache and dcache */
			ccselr = (level << 1) | 1;
			write_csselr(ccselr);
			ccsidr = read_ccsidr();
			printk(BIOS_DEBUG, "L%d: icache 0x%08x, ", level + 1, ccsidr);
			nsets = ((ccsidr & __mask(27, 13)) >> 13) + 1;
			assoc = ((ccsidr & __mask(12, 3)) >> 3) + 1;
			linesize = (1 << ((ccsidr & 0x7) + 2)) * 4;
			printk(BIOS_DEBUG, "nsets: %d, assoc: %d, linesize: %d\n", nsets, assoc, linesize);
			ccselr = ccsidr = 0;	/* FIXME: paranoia */
			ccselr = level << 1;
			write_csselr(ccselr);
			ccsidr = read_ccsidr();
			printk(BIOS_DEBUG, "L%d: dcache 0x%08x, ", level + 1, ccsidr);
			nsets = ((ccsidr & __mask(27, 13)) >> 13) + 1;
			assoc = ((ccsidr & __mask(12, 3)) >> 3) + 1;
			linesize = (1 << ((ccsidr & 0x7) + 2)) * 4;
			printk(BIOS_DEBUG, "nsets: %d, assoc: %d, linesize: %d\n", nsets, assoc, linesize);
			break;
		case 0x4:
			/* unified cache */
			ccselr = level << 1;
			write_csselr(ccselr);
			ccsidr = read_ccsidr();
			printk(BIOS_DEBUG, "L%d: unified cache 0x%08x, ", level + 1, ccsidr);
			nsets = ((ccsidr & __mask(27, 13)) >> 13) + 1;
			assoc = ((ccsidr & __mask(12, 3)) >> 3) + 1;
			linesize = (1 << ((ccsidr & 0x7) + 2)) * 4;
			printk(BIOS_DEBUG, "nsets: %d, assoc: %d, linesize: %d\n", nsets, assoc, linesize);
			break;
		default:
			/* reserved */
			printk(BIOS_DEBUG, "L%d has unknown cache\n", level + 1);
			break;
		}
	}
}

void hardwaremain(int boot_complete);
void main(void)
{
	console_init();
	printk(BIOS_INFO, "hello from ramstage; now with deluxe exception handling.\n");

	foo();

	/* this is going to move, but we must have it now and we're not sure where */
	exception_init();
	/* place at top of physical memory */
	high_tables_size = CONFIG_COREBOOT_TABLES_SIZE;
	high_tables_base = CONFIG_SYS_SDRAM_BASE +
			((CONFIG_DRAM_SIZE_MB << 20UL) * CONFIG_NR_DRAM_BANKS) -
			CONFIG_COREBOOT_TABLES_SIZE;

	const unsigned epll_hz = 192000000;
	const unsigned sample_rate = 48000;
	const unsigned lr_frame_size = 256;
	clock_epll_set_rate(epll_hz);
	clock_select_i2s_clk_source();
	clock_set_i2s_clk_prescaler(epll_hz, sample_rate * lr_frame_size);

	power_enable_xclkout();

	hardwaremain(0);
}
