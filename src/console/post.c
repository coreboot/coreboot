/*
 * This file is part of the coreboot project.
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
 */

#include <stdint.h>
#include <elog.h>
#include <console/console.h>
#include <device/device.h>
#include <pc80/mc146818rtc.h>
#include <smp/spinlock.h>
#if CONFIG(POST_IO)
#include <arch/io.h>
#endif

/* Write POST information */

/* Some mainboards have very nice features beyond just a simple display.
 * They can override this function.
 */
void __weak mainboard_post(uint8_t value)
{
}

#if CONFIG(CMOS_POST)

DECLARE_SPIN_LOCK(cmos_post_lock)

void cmos_post_log(void)
{
	u8 code = 0;
#if CONFIG(CMOS_POST_EXTRA)
	u32 extra = 0;
#endif

	spin_lock(&cmos_post_lock);

	/* Get post code from other bank */
	switch (cmos_read(CMOS_POST_BANK_OFFSET)) {
	case CMOS_POST_BANK_0_MAGIC:
		code = cmos_read(CMOS_POST_BANK_1_OFFSET);
#if CONFIG(CMOS_POST_EXTRA)
		extra = cmos_read32(CMOS_POST_BANK_1_EXTRA);
#endif
		break;
	case CMOS_POST_BANK_1_MAGIC:
		code = cmos_read(CMOS_POST_BANK_0_OFFSET);
#if CONFIG(CMOS_POST_EXTRA)
		extra = cmos_read32(CMOS_POST_BANK_0_EXTRA);
#endif
		break;
	}

	spin_unlock(&cmos_post_lock);

	/* Check last post code in previous boot against normal list */
	switch (code) {
	case POST_OS_BOOT:
	case POST_OS_RESUME:
	case POST_ENTER_ELF_BOOT:
	case 0:
		break;
	default:
		printk(BIOS_WARNING, "POST: Unexpected post code "
		       "in previous boot: 0x%02x\n", code);
#if CONFIG(ELOG) && (ENV_RAMSTAGE || CONFIG(ELOG_PRERAM))
		elog_add_event_word(ELOG_TYPE_LAST_POST_CODE, code);
#if CONFIG(CMOS_POST_EXTRA)
		if (extra)
			elog_add_event_dword(ELOG_TYPE_POST_EXTRA, extra);
#endif
#endif
	}
}

#if CONFIG(CMOS_POST_EXTRA)
void post_log_extra(u32 value)
{
	spin_lock(&cmos_post_lock);

	switch (cmos_read(CMOS_POST_BANK_OFFSET)) {
	case CMOS_POST_BANK_0_MAGIC:
		cmos_write32(CMOS_POST_BANK_0_EXTRA, value);
		break;
	case CMOS_POST_BANK_1_MAGIC:
		cmos_write32(CMOS_POST_BANK_1_EXTRA, value);
		break;
	}

	spin_unlock(&cmos_post_lock);
}

void post_log_path(const struct device *dev)
{
	if (dev) {
		/* Encode path into lower 3 bytes */
		u32 path = dev_path_encode(dev);
		/* Upper byte contains the log type */
		path |= CMOS_POST_EXTRA_DEV_PATH << 24;
		post_log_extra(path);
	}
}

void post_log_clear(void)
{
	post_log_extra(0);
}
#endif /* CONFIG_CMOS_POST_EXTRA */

static void cmos_post_code(u8 value)
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
#endif /* CONFIG_CMOS_POST */

void post_code(uint8_t value)
{
#if !CONFIG(NO_POST)
#if CONFIG(CONSOLE_POST)
	printk(BIOS_EMERG, "POST: 0x%02x\n", value);
#endif
#if CONFIG(CMOS_POST)
	cmos_post_code(value);
#endif
#if CONFIG(POST_IO)
	outb(value, CONFIG_POST_IO_PORT);
#endif
#endif
	mainboard_post(value);
}
