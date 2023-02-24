/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <ip_checksum.h>
#include <intelbasecode/tom.h>
#include <pc80/mc146818rtc.h>
#include <stdint.h>

/* We need a region in CMOS to store the TOM address */

#define TOM_SIGNATURE   0x5F544F4D /* '_TOM' */

#define TOM_CMOS_OFFSET 0x64

/*
 * Address of the tom_cmos_offset byte in CMOS. Should be reserved
 * in mainboards' cmos.layout and not covered by checksum.
 */

#if CONFIG(USE_OPTION_TABLE)
#include "option_table.h"
#if CMOS_VSTART_tom_cmos_offset != TOM_CMOS_OFFSET * 8
#error "CMOS start for TOM_CMOS is not correct, check your cmos.layout"
#endif
#if CMOS_VLEN_tom_cmos_offset != 12
#error "CMOS length for TOM_CMOS bytes are not correct, check your cmos.layout"
#endif
#endif

struct tom_table {
	uint32_t signature;
	uint32_t addr;
	uint16_t checksum;
} __packed;

/* Read and validate tom_table structure from CMOS */
static int tom_cmos_read(struct tom_table *tom)
{
	u8 i, *p;
	u16 csum;

	for (p = (u8 *)tom, i = 0; i < sizeof(*tom); i++, p++)
		*p = cmos_read(TOM_CMOS_OFFSET + i);

	/* Verify signature */
	if (tom->signature != TOM_SIGNATURE) {
		printk(BIOS_DEBUG, "tom_table invalid signature\n");
		return -1;
	}

	/* Verify checksum over signature and counter only */
	csum = compute_ip_checksum(tom, offsetof(struct tom_table, checksum));

	if (csum != tom->checksum) {
		printk(BIOS_DEBUG, "tom_table checksum mismatch\n");
		return -1;
	}

	return 0;
}

/* Write tom_table structure to CMOS */
static void tom_cmos_write(struct tom_table *tom)
{
	u8 i, *p;

	/* Checksum over signature and counter only */
	tom->checksum = compute_ip_checksum(
		tom, offsetof(struct tom_table, checksum));

	for (p = (u8 *)tom, i = 0; i < sizeof(*tom); i++, p++)
		cmos_write(*p, TOM_CMOS_OFFSET + i);
}

/* Update the TOM if required based on the input TOM address */
void update_tom(uint32_t addr)
{
	struct tom_table tom;

	/* Read and increment boot count */
	if (tom_cmos_read(&tom) < 0) {
		/* Structure invalid, re-initialize */
		tom.signature = TOM_SIGNATURE;
		tom.addr = 0;
	}

	/* Update TOM if required */
	if (tom.addr == addr)
		return;

	tom.addr = addr;

	/* Write the new count to CMOS */
	tom_cmos_write(&tom);

	printk(BIOS_DEBUG, "Updated the TOM address into CMOS 0x%x\n", tom.addr);
}

static uint32_t get_tom_addr(void)
{
	struct tom_table tom;

	if (tom_cmos_read(&tom) < 0)
		return 0;

	return tom.addr;
}

/* Early caching of TOM region */
void early_tom_enable_cache_range(void)
{
	uint32_t tom = get_tom_addr();
	if (!tom)
		return;

	int mtrr = get_free_var_mtrr();
	if (mtrr == -1) {
		printk(BIOS_WARNING, "tom_table update failure due to no free MTRR available!\n");
		return;
	}
	/*
	 * We need to make sure late romstage (including FSP-M post mem) will be run
	 * cached. Caching 16MB below TOM is a safe to cover late romstage.
	 */
	set_var_mtrr(mtrr, tom - 16 * MiB, 16 * MiB, MTRR_TYPE_WRBACK);
}
