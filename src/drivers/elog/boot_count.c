/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <ip_checksum.h>
#include <pc80/mc146818rtc.h>
#include <stdint.h>
#include <elog.h>

/*
 * We need a region in CMOS to store the boot counter.
 *
 * This can either be declared as part of the option
 * table or statically defined in the board config.
 */
#if CONFIG(USE_OPTION_TABLE)
# include "option_table.h"
# define BOOT_COUNT_CMOS_OFFSET (CMOS_VSTART_boot_count_offset >> 3)
#else
# if (CONFIG_ELOG_BOOT_COUNT_CMOS_OFFSET != 0)
#  define BOOT_COUNT_CMOS_OFFSET CONFIG_ELOG_BOOT_COUNT_CMOS_OFFSET
# else
#  error "Must configure CONFIG_ELOG_BOOT_COUNT_CMOS_OFFSET"
# endif
#endif

#define BOOT_COUNT_SIGNATURE 0x4342 /* 'BC' */

struct boot_count {
	u16 signature;
	u32 count;
	u16 checksum;
} __packed;

/* Read and validate boot count structure from CMOS */
static int boot_count_cmos_read(struct boot_count *bc)
{
	u8 i, *p;
	u16 csum;

	for (p = (u8 *)bc, i = 0; i < sizeof(*bc); i++, p++)
		*p = cmos_read(BOOT_COUNT_CMOS_OFFSET + i);

	/* Verify signature */
	if (bc->signature != BOOT_COUNT_SIGNATURE) {
		printk(BIOS_DEBUG, "Boot Count invalid signature\n");
		return -1;
	}

	/* Verify checksum over signature and counter only */
	csum = compute_ip_checksum(bc, offsetof(struct boot_count, checksum));

	if (csum != bc->checksum) {
		printk(BIOS_DEBUG, "Boot Count checksum mismatch\n");
		return -1;
	}

	return 0;
}

/* Write boot count structure to CMOS */
static void boot_count_cmos_write(struct boot_count *bc)
{
	u8 i, *p;

	/* Checksum over signature and counter only */
	bc->checksum = compute_ip_checksum(
		bc, offsetof(struct boot_count, checksum));

	for (p = (u8 *)bc, i = 0; i < sizeof(*bc); i++, p++)
		cmos_write(*p, BOOT_COUNT_CMOS_OFFSET + i);
}

/* Increment boot count and return the new value */
u32 boot_count_increment(void)
{
	struct boot_count bc;

	/* Read and increment boot count */
	if (boot_count_cmos_read(&bc) < 0) {
		/* Structure invalid, re-initialize */
		bc.signature = BOOT_COUNT_SIGNATURE;
		bc.count = 0;
	}

	/* Increment boot counter */
	bc.count++;

	/* Write the new count to CMOS */
	boot_count_cmos_write(&bc);

	printk(BIOS_DEBUG, "Boot Count incremented to %u\n", bc.count);
	return bc.count;
}

/* Return the current boot count */
u32 boot_count_read(void)
{
	struct boot_count bc;

	if (boot_count_cmos_read(&bc) < 0)
		return 0;

	return bc.count;
}
