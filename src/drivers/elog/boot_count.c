/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/ipchksum.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <fmap.h>
#include <pc80/mc146818rtc.h>
#include <stdint.h>
#include <elog.h>

#if !CONFIG(ELOG_BOOT_COUNT_FLASH)
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
#else
# define BOOT_COUNT_CMOS_OFFSET 0
#endif

#define BOOT_COUNT_SIGNATURE 0x4342 /* 'BC' */

struct boot_count {
	u16 signature;
	u32 count;
	u16 checksum;
} __packed;

#define RW_BC_REGION_NAME "RW_BC"

struct boot_count_flash_ctx {
	int initialized;
	struct region_device rdev;
};
static struct boot_count_flash_ctx bc_ctx;

/* Initialize the region device for RW_BC */
static int init_rw_bc(void)
{
	if (bc_ctx.initialized)
		return 0;

	if (fmap_locate_area_as_rdev_rw(RW_BC_REGION_NAME, &bc_ctx.rdev)) {
		printk(BIOS_ERR, "BC: Failed to locate %s region\n", RW_BC_REGION_NAME);
		return -1;
	}

	if (region_device_sz(&bc_ctx.rdev) < sizeof(struct boot_count)) {
		printk(BIOS_ERR, "BC: %s region is too small (%zu < %zu)\n",
		       RW_BC_REGION_NAME, region_device_sz(&bc_ctx.rdev),
		       sizeof(struct boot_count));
		return -1;
	}

	bc_ctx.initialized = 1;
	return 0;
}

/* Read data from the RW_BC flash region */
static int read_from_rw_bc(void *buffer, size_t size)
{
	if (!bc_ctx.initialized) {
		if (init_rw_bc() != 0)
			return -1;
	}

	if (rdev_readat(&bc_ctx.rdev, buffer, 0, size) < 0) {
		printk(BIOS_ERR, "BC: Failed to read from %s\n", RW_BC_REGION_NAME);
		return -1;
	}
	return 0;
}

/* Write data to the RW_BC flash region */
static int write_to_rw_bc(const void *buffer, size_t size)
{
	if (!bc_ctx.initialized) {
		if (init_rw_bc() != 0)
			return -1;
	}

	// Erase the area
	if (rdev_eraseat(&bc_ctx.rdev, 0, region_device_sz(&bc_ctx.rdev)) < 0) {
		printk(BIOS_ERR, "BC: Failed to erase %s at offset %d size %zu\n",
		       RW_BC_REGION_NAME, 0, size);
		return -1;
	}

	// Write the data
	if (rdev_writeat(&bc_ctx.rdev, buffer, 0, size) != size) {
		printk(BIOS_ERR, "BC: Failed to write to %s at offset %d size %zu\n",
		       RW_BC_REGION_NAME, 0, size);
		return -1;
	}
	return 0;
}

/* Read and validate boot count structure from CMOS */
static int boot_count_backend_read(struct boot_count *bc)
{
	u16 csum;

	if (CONFIG(ELOG_BOOT_COUNT_FLASH)) {
		if (read_from_rw_bc(bc, sizeof(*bc)) != 0) {
			printk(BIOS_DEBUG, "Boot Count: Flash read failed from %s\n", RW_BC_REGION_NAME);
			return -1;
		}
	} else {
		u8 i, *p;
		for (p = (u8 *)bc, i = 0; i < sizeof(*bc); i++, p++)
			*p = cmos_read(BOOT_COUNT_CMOS_OFFSET + i);
	}

	/* Verify signature */
	if (bc->signature != BOOT_COUNT_SIGNATURE) {
		printk(BIOS_DEBUG, "Boot Count invalid signature\n");
		return -1;
	}

	/* Verify checksum over signature and counter only */
	csum = ipchksum(bc, offsetof(struct boot_count, checksum));

	if (csum != bc->checksum) {
		printk(BIOS_DEBUG, "Boot Count checksum mismatch\n");
		return -1;
	}

	return 0;
}

/* Write boot count structure to CMOS */
static void boot_count_backend_write(struct boot_count *bc)
{
	/* Checksum over signature and counter only */
	bc->checksum = ipchksum(
		bc, offsetof(struct boot_count, checksum));

	if (CONFIG(ELOG_BOOT_COUNT_FLASH)) {
		if (write_to_rw_bc(bc, sizeof(*bc)) != 0)
			printk(BIOS_DEBUG, "Boot Count: Flash write failed to %s\n", RW_BC_REGION_NAME);
	} else {
		u8 i, *p;
		for (p = (u8 *)bc, i = 0; i < sizeof(*bc); i++, p++)
			cmos_write(*p, BOOT_COUNT_CMOS_OFFSET + i);
	}
}

/* Increment boot count and return the new value */
u32 boot_count_increment(void)
{
	struct boot_count bc;

	/* Read and increment boot count */
	if (boot_count_backend_read(&bc) < 0) {
		/* Structure invalid, re-initialize */
		bc.signature = BOOT_COUNT_SIGNATURE;
		bc.count = 0;
	}

	/* Increment boot counter */
	bc.count++;

	/* Write the new count to CMOS */
	boot_count_backend_write(&bc);

	printk(BIOS_DEBUG, "Boot Count incremented to %u\n", bc.count);
	return bc.count;
}

/* Return the current boot count */
u32 boot_count_read(void)
{
	struct boot_count bc;

	if (boot_count_backend_read(&bc) < 0)
		return 0;

	return bc.count;
}
