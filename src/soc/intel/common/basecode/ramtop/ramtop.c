/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <ip_checksum.h>
#include <intelbasecode/ramtop.h>
#include <pc80/mc146818rtc.h>
#include <stdint.h>

/* We need a region in CMOS to store the RAMTOP address */

#define RAMTOP_SIGNATURE   0x52544F50 /* 'RTOP' */

#define RAMTOP_CMOS_OFFSET 0x64

/*
 * Address of the ramtop byte in CMOS. Should be reserved
 * in mainboards' cmos.layout and not covered by checksum.
 */

#if CONFIG(USE_OPTION_TABLE)
#include "option_table.h"
#if CMOS_VSTART_ramtop != RAMTOP_CMOS_OFFSET * 8
#error "CMOS start for RAMTOP_CMOS is not correct, check your cmos.layout"
#endif
#if CMOS_VLEN_ramtop != 12
#error "CMOS length for RAMTOP_CMOS bytes are not correct, check your cmos.layout"
#endif
#endif

struct ramtop_table {
	uint32_t signature;
	uint32_t addr;
	uint16_t checksum;
} __packed;

/* Read and validate ramtop_table structure from CMOS */
static int ramtop_cmos_read(struct ramtop_table *ramtop)
{
	u8 i, *p;
	u16 csum;

	for (p = (u8 *)ramtop, i = 0; i < sizeof(*ramtop); i++, p++)
		*p = cmos_read(RAMTOP_CMOS_OFFSET + i);

	/* Verify signature */
	if (ramtop->signature != RAMTOP_SIGNATURE) {
		printk(BIOS_DEBUG, "ramtop_table invalid signature\n");
		return -1;
	}

	/* Verify checksum over signature and counter only */
	csum = compute_ip_checksum(ramtop, offsetof(struct ramtop_table, checksum));

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
	ramtop->checksum = compute_ip_checksum(
		ramtop, offsetof(struct ramtop_table, checksum));

	for (p = (u8 *)ramtop, i = 0; i < sizeof(*ramtop); i++, p++)
		cmos_write(*p, RAMTOP_CMOS_OFFSET + i);
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
	}

	/* Update ramtop if required */
	if (ramtop.addr == addr)
		return;

	ramtop.addr = addr;

	/* Write the new top_of_ram address to CMOS */
	ramtop_cmos_write(&ramtop);

	printk(BIOS_DEBUG, "Updated the RAMTOP address into CMOS 0x%x\n", ramtop.addr);
}

static uint32_t get_ramtop_addr(void)
{
	struct ramtop_table ramtop;

	if (ramtop_cmos_read(&ramtop) < 0)
		return 0;

	return ramtop.addr;
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
	/*
	 * We need to make sure late romstage (including FSP-M post mem) will be run
	 * cached. Caching 16MB below ramtop is a safe to cover late romstage.
	 */
	set_var_mtrr(mtrr, ramtop - 16 * MiB, 16 * MiB, MTRR_TYPE_WRBACK);
}
