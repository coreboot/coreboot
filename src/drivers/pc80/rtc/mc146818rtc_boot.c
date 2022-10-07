/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <fallback.h>
#include <pc80/mc146818rtc.h>
#include <stdint.h>

#if  CONFIG_MAX_REBOOT_CNT > 15
#error "CONFIG_MAX_REBOOT_CNT too high"
#endif

static int boot_count(uint8_t rtc_byte)
{
	return rtc_byte >> 4;
}

static uint8_t increment_boot_count(uint8_t rtc_byte)
{
	return rtc_byte + (1 << 4);
}

static uint8_t boot_set_fallback(uint8_t rtc_byte)
{
	return rtc_byte & ~RTC_BOOT_NORMAL;
}

static int boot_use_normal(uint8_t rtc_byte)
{
	return rtc_byte & RTC_BOOT_NORMAL;
}

int do_normal_boot(void)
{
	unsigned char byte;

	if (cmos_error() || (CONFIG(USE_OPTION_TABLE) && !cmos_lb_cks_valid())) {
		printk(BIOS_WARNING,
		       "Invalid CMOS checksum detected! Force fallback boot...\n");
		byte = cmos_read(RTC_BOOT_BYTE);
		byte &= boot_set_fallback(byte) & 0x0f;
		byte |= 0xf << 4;
		cmos_write(byte, RTC_BOOT_BYTE);
	}

	/* The RTC_BOOT_BYTE is now o.k. see where to go. */
	byte = cmos_read(RTC_BOOT_BYTE);

	/* Are we attempting to boot normally? */
	if (boot_use_normal(byte)) {
		/* Are we already at the max count? */
		if (boot_count(byte) < CONFIG_MAX_REBOOT_CNT)
			byte = increment_boot_count(byte);
		else
			byte = boot_set_fallback(byte);
	}

	/* Save the boot byte */
	cmos_write(byte, RTC_BOOT_BYTE);

	/* Return selected code path for this boot attempt */
	return boot_use_normal(byte);
}
