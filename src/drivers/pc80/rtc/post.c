/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <stdint.h>
#include <console/console.h>
#include <device/device.h>
#include <pc80/mc146818rtc.h>
#include <smp/spinlock.h>

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
		if (CONFIG(CMOS_POST_EXTRA))
			*extra = cmos_read32(CMOS_POST_BANK_1_EXTRA);
		break;
	case CMOS_POST_BANK_1_MAGIC:
		*code = cmos_read(CMOS_POST_BANK_0_OFFSET);
		if (CONFIG(CMOS_POST_EXTRA))
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
		if (CONFIG(CMOS_POST_EXTRA)) {
			cmos_write32(0, CMOS_POST_BANK_0_EXTRA);
			cmos_write32(0, CMOS_POST_BANK_1_EXTRA);
		}
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

static void __unused cmos_post_extra(u32 value)
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

#if CONFIG(CMOS_POST_EXTRA)
void post_log_path(const struct device *dev)
{
	if (dev) {
		/* Encode path into lower 3 bytes */
		u32 path = dev_path_encode(dev);
		/* Upper byte contains the log type */
		path |= CMOS_POST_EXTRA_DEV_PATH << 24;
		cmos_post_extra(path);
	}
}

void post_log_clear(void)
{
	cmos_post_extra(0);
}
#endif /* CONFIG_CMOS_POST_EXTRA */
