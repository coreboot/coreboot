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
#ifdef __ROMCC__
#include <arch/cbfs.h>
#else
#include <cbfs.h>
#endif
#include <pc80/mc146818rtc.h>
#if IS_ENABLED(CONFIG_USE_OPTION_TABLE)
#include <option_table.h>
#endif

int cmos_error(void);
int cmos_error(void)
{
	unsigned char reg_d;
	/* See if the cmos error condition has been flagged */
	reg_d = cmos_read(RTC_REG_D);
	return (reg_d & RTC_VRT) == 0;
}

int cmos_chksum_valid(void);
int cmos_chksum_valid(void)
{
#if IS_ENABLED(CONFIG_USE_OPTION_TABLE)
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

#if IS_ENABLED(CONFIG_USE_OPTION_TABLE)
void sanitize_cmos(void)
{
	if (cmos_error() || !cmos_chksum_valid() ||
	    IS_ENABLED(CONFIG_STATIC_OPTION_TABLE)) {
		size_t length = 128;
		const unsigned char *cmos_default =
#ifdef __ROMCC__
			walkcbfs("cmos.default");
#else
			cbfs_boot_map_with_leak("cmos.default",
					CBFS_COMPONENT_CMOS_DEFAULT, &length);
#endif
		if (cmos_default) {
			int i;
			cmos_disable_rtc();
			for (i = 14; i < MIN(128, length); i++)
				cmos_write_inner(cmos_default[i], i);
			cmos_enable_rtc();
		}
	}
}
#endif
