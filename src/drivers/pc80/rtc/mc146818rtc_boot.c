/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <cbfs.h>
#include <pc80/mc146818rtc.h>
#include <fallback.h>
#if CONFIG(USE_OPTION_TABLE)
#include <option_table.h>
#endif

int cmos_error(void)
{
	unsigned char reg_d;
	/* See if the cmos error condition has been flagged */
	reg_d = cmos_read(RTC_REG_D);
	return (reg_d & RTC_VRT) == 0;
}

int cmos_chksum_valid(void)
{
#if CONFIG(USE_OPTION_TABLE)
	unsigned char addr;
	u16 sum, old_sum;

	sum = 0;
	/* Compute the cmos checksum */
	for (addr = LB_CKS_RANGE_START; addr <= LB_CKS_RANGE_END; addr++)
		sum += cmos_read(addr);

	/* Read the stored checksum */
	old_sum = cmos_read(LB_CKS_LOC) << 8;
	old_sum |= cmos_read(LB_CKS_LOC + 1);

	return sum == old_sum;
#else
	return 0;
#endif
}

#if CONFIG(USE_OPTION_TABLE)
void sanitize_cmos(void)
{
	if (cmos_error() || !cmos_chksum_valid() ||
	    CONFIG(STATIC_OPTION_TABLE)) {
		size_t length = 128;
		const unsigned char *cmos_default =
			cbfs_boot_map_with_leak("cmos.default",
					CBFS_COMPONENT_CMOS_DEFAULT, &length);
		if (cmos_default) {
			size_t i;
			cmos_disable_rtc();
			for (i = 14; i < MIN(128, length); i++)
				cmos_write_inner(cmos_default[i], i);
			cmos_enable_rtc();
		}
	}
}
#endif

#if  CONFIG_MAX_REBOOT_CNT > 15
#error "CONFIG_MAX_REBOOT_CNT too high"
#endif

static inline int boot_count(uint8_t rtc_byte)
{
	return rtc_byte >> 4;
}

static inline uint8_t increment_boot_count(uint8_t rtc_byte)
{
	return rtc_byte + (1 << 4);
}

static inline uint8_t boot_set_fallback(uint8_t rtc_byte)
{
	return rtc_byte & ~RTC_BOOT_NORMAL;
}

static inline int boot_use_normal(uint8_t rtc_byte)
{
	return rtc_byte & RTC_BOOT_NORMAL;
}

int do_normal_boot(void)
{
	unsigned char byte;

	if (cmos_error() || !cmos_chksum_valid()) {
		/* Invalid CMOS checksum detected!
		 * Force fallback boot...
		 */
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
