/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/bsd/ipchksum.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <fsp/util.h>
#include <intelbasecode/ramtop.h>
#include <pc80/mc146818rtc.h>
#include <stdint.h>

/* We need a region in CMOS to store the RAMTOP address */

#define RAMTOP_SIGNATURE   0x504F5452 /* 'RTOP' */

/*
 * Address of the ramtop byte in CMOS. Should be reserved
 * in mainboards' cmos.layout and not covered by checksum.
 */

#if CONFIG(USE_OPTION_TABLE)
#include "option_table.h"

#ifndef CMOS_VSTART_ramtop
#error "The `ramtop` CMOS entry is missing, please add it to your cmos.layout."
#endif

#if CMOS_VSTART_ramtop % 8 != 0
#error "The `ramtop` CMOS entry needs to be byte aligned, check your cmos.layout."
#endif	// CMOS_VSTART_ramtop % 8 != 0

#if CMOS_VLEN_ramtop != (10 * 8)
#error "The `ramtop` CMOS entry needs to be 10 bytes long, check your cmos.layout."
#endif	// CMOS_VLEN_ramtop != (10 * 8)

#else
#define CMOS_VSTART_ramtop 800
#endif	// CONFIG(USE_OPTION_TABLE)

struct ramtop_table {
	uint32_t signature;
	uint32_t addr;
	size_t size;
	uint16_t checksum;
} __packed;

/* Read and validate ramtop_table structure from CMOS */
static int ramtop_cmos_read(struct ramtop_table *ramtop)
{
	u8 i, *p;
	u16 csum;

	for (p = (u8 *)ramtop, i = 0; i < sizeof(*ramtop); i++, p++)
		*p = cmos_read((CMOS_VSTART_ramtop / 8) + i);

	/* Verify signature */
	if (ramtop->signature != RAMTOP_SIGNATURE) {
		printk(BIOS_DEBUG, "ramtop_table invalid signature\n");
		return -1;
	}

	/* Verify RAMTOP size */
	if (ramtop->size == 0) {
		printk(BIOS_DEBUG, "ramtop_table holds invalid size\n");
		return -1;
	}

	/* Verify checksum over signature and counter only */
	csum = ipchksum(ramtop, offsetof(struct ramtop_table, checksum));

	if (csum != ramtop->checksum) {
		printk(BIOS_DEBUG, "ramtop_table checksum mismatch\n");
		return -1;
	}

	return 0;
}

/* Write ramtop_table structure to CMOS */
static void ramtop_cmos_write(struct ramtop_table *ramtop)
{
	u8 i, *p;

	/* Checksum over signature and counter only */
	ramtop->checksum = ipchksum(ramtop, offsetof(struct ramtop_table, checksum));

	for (p = (u8 *)ramtop, i = 0; i < sizeof(*ramtop); i++, p++)
		cmos_write(*p, (CMOS_VSTART_ramtop / 8) + i);
}

/*
 * RAMTOP range:
 *
 *  This defines the memory range covered by RAMTOP, which extends from
 *  cbmem_top down to FSP TOLUM. This range includes essential components:
 *
 * +---------------------------+ TOLUM / top_of_ram / cbmem_top
 * | CBMEM Root                |
 * +---------------------------+
 * | FSP Reserved Memory       |
 * +---------------------------+
 * | various CBMEM entries     |
 * +---------------------------+ top_of_stack (8 byte aligned)
 * | stack (CBMEM entry)       |
 * +---------------------------+ FSP TOLUM
 * |                           |
 * +---------------------------+ 0
*/
static size_t calculate_ramtop_size(uint32_t addr)
{
	struct range_entry fsp_mem;
	uint32_t fsp_reserve_base;
	fsp_find_reserved_memory(&fsp_mem);

	fsp_reserve_base = range_entry_base(&fsp_mem);
	size_t ramtop_size = ALIGN_UP(addr - fsp_reserve_base, 4 * MiB);

	return ramtop_size;
}

/* Update the RAMTOP if required based on the input top_of_ram address */
void update_ramtop(uint32_t addr)
{
	struct ramtop_table ramtop;

	/* Read and update ramtop (if required) */
	if (ramtop_cmos_read(&ramtop) < 0) {
		/* Structure invalid, re-initialize */
		ramtop.signature = RAMTOP_SIGNATURE;
		ramtop.addr = 0;
		ramtop.size = 0;
	}

	size_t size = calculate_ramtop_size(addr);

	/* Update ramtop if required */
	if ((ramtop.addr == addr) && (ramtop.size == size))
		return;

	ramtop.addr = addr;
	ramtop.size = size;

	/* Write the new top_of_ram address to CMOS */
	ramtop_cmos_write(&ramtop);

	printk(BIOS_DEBUG, "Updated the RAMTOP address (0x%x) with size (0x%zx) into CMOS\n",
			 ramtop.addr, ramtop.size);
}

uint32_t get_ramtop_addr(void)
{
	struct ramtop_table ramtop;

	if (ramtop_cmos_read(&ramtop) < 0)
		return 0;

	return ramtop.addr;
}

static uint32_t get_ramtop_size(void)
{
	struct ramtop_table ramtop;

	if (ramtop_cmos_read(&ramtop) < 0)
		return 0;

	return ramtop.size;
}

/* Early caching of top_of_ram region */
void early_ramtop_enable_cache_range(void)
{
	uint32_t ramtop = get_ramtop_addr();
	if (!ramtop)
		return;

	int mtrr = get_free_var_mtrr();
	if (mtrr == -1) {
		printk(BIOS_WARNING, "ramtop_table update failure due to no free MTRR available!\n");
		return;
	}

	size_t ramtop_size = get_ramtop_size();
	/*
	 * Background: Some SoCs have a critical bug inside the NEM logic which is responsible
	 *             for mapping cached memory to physical memory during tear down and
	 *             eventually malfunctions if the number of cache sets is not a power of two.
	 *             This can lead to runtime hangs.
	 *
	 * Workaround: To mitigate this issue on affected SoCs, we force the MTRR type to
	 *             WC (Write Combining) unless the cache set count is a power of two.
	 *             This change alters caching behavior but prevents the runtime failures.
	 */
	unsigned int mtrr_type = MTRR_TYPE_WRCOMB;
	/*
	* Late romstage (including FSP-M post-memory initialization) needs to be
	* executed from cache for performance reasons. This requires caching
	* `ramtop_size`, which encompasses both FSP reserved memory and the CBMEM
	* range, to guarantee sufficient cache coverage for late romstage.
	*/
	if (is_cache_sets_power_of_two())
		mtrr_type = MTRR_TYPE_WRBACK;

	set_var_mtrr(mtrr, ramtop - ramtop_size, ramtop_size, mtrr_type);
}
