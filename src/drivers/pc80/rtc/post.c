/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <stdint.h>
#include <post.h>
#include <console/console.h>
#include <device/device.h>
#include <pc80/mc146818rtc.h>
#include <smp/spinlock.h>

#if CONFIG(USE_OPTION_TABLE)
# include "option_table.h"
# define CMOS_POST_OFFSET (CMOS_VSTART_cmos_post_offset >> 3)
#else
# if (CONFIG_CMOS_POST_OFFSET != 0)
#  define CMOS_POST_OFFSET CONFIG_CMOS_POST_OFFSET
# else
#  error "Must configure CONFIG_CMOS_POST_OFFSET"
# endif
#endif

/*
 *    0 = Bank Select Magic
 *    1 = Bank 0 POST
 *    2 = Bank 1 POST
 *  3-6 = BANK 0 Extra log
 * 7-10 = BANK 1 Extra log
 */
#define CMOS_POST_BANK_OFFSET     (CMOS_POST_OFFSET)
#define CMOS_POST_BANK_0_MAGIC    0x80
#define CMOS_POST_BANK_0_OFFSET   (CMOS_POST_OFFSET + 1)
#define CMOS_POST_BANK_0_EXTRA    (CMOS_POST_OFFSET + 3)
#define CMOS_POST_BANK_1_MAGIC    0x81
#define CMOS_POST_BANK_1_OFFSET   (CMOS_POST_OFFSET + 2)
#define CMOS_POST_BANK_1_EXTRA    (CMOS_POST_OFFSET + 7)

#define CMOS_POST_EXTRA_DEV_PATH  0x01

DECLARE_SPIN_LOCK(cmos_post_lock)

int cmos_post_previous_boot(u8 *code, u32 *extra)
{
	*code = 0;
	*extra = 0;

	spin_lock(&cmos_post_lock);

	/* Get post code from other bank */
	switch (cmos_read(CMOS_POST_BANK_OFFSET)) {
	case CMOS_POST_BANK_0_MAGIC:
		*code = cmos_read(CMOS_POST_BANK_1_OFFSET);
		*extra = cmos_read32(CMOS_POST_BANK_1_EXTRA);
		break;
	case CMOS_POST_BANK_1_MAGIC:
		*code = cmos_read(CMOS_POST_BANK_0_OFFSET);
		*extra = cmos_read32(CMOS_POST_BANK_0_EXTRA);
		break;
	}

	spin_unlock(&cmos_post_lock);

	/* Check last post code in previous boot against normal list */
	switch (*code) {
	case POST_OS_BOOT:
	case POST_OS_RESUME:
	case POST_ENTER_ELF_BOOT:
	case 0:
		break;
	default:
		return -1;
	}

	return 0;
}

void cmos_post_init(void)
{
	u8 magic = CMOS_POST_BANK_0_MAGIC;

	/* Switch to the other bank */
	switch (cmos_read(CMOS_POST_BANK_OFFSET)) {
	case CMOS_POST_BANK_1_MAGIC:
		break;
	case CMOS_POST_BANK_0_MAGIC:
		magic = CMOS_POST_BANK_1_MAGIC;
		break;
	default:
		/* Initialize to zero */
		cmos_write(0, CMOS_POST_BANK_0_OFFSET);
		cmos_write(0, CMOS_POST_BANK_1_OFFSET);
		cmos_write32(0, CMOS_POST_BANK_0_EXTRA);
		cmos_write32(0, CMOS_POST_BANK_1_EXTRA);
	}

	cmos_write(magic, CMOS_POST_BANK_OFFSET);
}

void cmos_post_code(u8 value)
{
	spin_lock(&cmos_post_lock);

	switch (cmos_read(CMOS_POST_BANK_OFFSET)) {
	case CMOS_POST_BANK_0_MAGIC:
		cmos_write(value, CMOS_POST_BANK_0_OFFSET);
		break;
	case CMOS_POST_BANK_1_MAGIC:
		cmos_write(value, CMOS_POST_BANK_1_OFFSET);
		break;
	}

	spin_unlock(&cmos_post_lock);
}

void cmos_post_extra(u32 value)
{
	spin_lock(&cmos_post_lock);

	switch (cmos_read(CMOS_POST_BANK_OFFSET)) {
	case CMOS_POST_BANK_0_MAGIC:
		cmos_write32(value, CMOS_POST_BANK_0_EXTRA);
		break;
	case CMOS_POST_BANK_1_MAGIC:
		cmos_write32(value, CMOS_POST_BANK_1_EXTRA);
		break;
	}

	spin_unlock(&cmos_post_lock);
}

void cmos_post_path(const struct device *dev)
{
	/* Encode path into lower 3 bytes */
	u32 path = dev_path_encode(dev);
	/* Upper byte contains the log type */
	path |= CMOS_POST_EXTRA_DEV_PATH << 24;
	cmos_post_extra(path);
}
