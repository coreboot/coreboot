#include <stdint.h>
#include <pc80/mc146818rtc.h>
#include <fallback.h>
#if CONFIG_USE_OPTION_TABLE
#include "option_table.h"
#endif

#ifndef CONFIG_MAX_REBOOT_CNT
#error "CONFIG_MAX_REBOOT_CNT not defined"
#endif
#if  CONFIG_MAX_REBOOT_CNT > 15
#error "CONFIG_MAX_REBOOT_CNT too high"
#endif

static int cmos_error(void)
{
	unsigned char reg_d;
	/* See if the cmos error condition has been flagged */
	reg_d = cmos_read(RTC_REG_D);
	return (reg_d & RTC_VRT) == 0;
}

static int cmos_chksum_valid(void)
{
#if CONFIG_USE_OPTION_TABLE
	unsigned char addr;
	u16 sum, old_sum;
	sum = 0;
	/* Compute the cmos checksum */
	for(addr = LB_CKS_RANGE_START; addr <= LB_CKS_RANGE_END; addr++) {
		sum += cmos_read(addr);
	}

	/* Read the stored checksum */
	old_sum = cmos_read(LB_CKS_LOC) << 8;
	old_sum |=  cmos_read(LB_CKS_LOC+1);

	return sum == old_sum;
#else
	return 0;
#endif
}


static inline int last_boot_normal(void)
{
	unsigned char byte;
	byte = cmos_read(RTC_BOOT_BYTE);
	return (byte & (1 << 1));
}

static inline int do_normal_boot(void)
{
	unsigned char byte;

	if (cmos_error() || !cmos_chksum_valid()) {
		/* There are no impossible values, no checksums so just
		 * trust whatever value we have in the the cmos,
		 * but clear the fallback bit.
		 */
		byte = cmos_read(RTC_BOOT_BYTE);
		byte &= 0x0c;
		byte |= CONFIG_MAX_REBOOT_CNT << 4;
		cmos_write(byte, RTC_BOOT_BYTE);
	}

	/* The RTC_BOOT_BYTE is now o.k. see where to go. */
	byte = cmos_read(RTC_BOOT_BYTE);

	/* Are we in normal mode? */
	if (byte & 1) {
		byte &= 0x0f; /* yes, clear the boot count */
	}

	/* Properly set the last boot flag */
	byte &= 0xfc;
	if ((byte >> 4) < CONFIG_MAX_REBOOT_CNT) {
		byte |= (1<<1);
	}

	/* Are we already at the max count? */
	if ((byte >> 4) < CONFIG_MAX_REBOOT_CNT) {
		byte += 1 << 4; /* No, add 1 to the count */
	}
	else {
		byte &= 0xfc;	/* Yes, put in fallback mode */
	}

	/* Save the boot byte */
	cmos_write(byte, RTC_BOOT_BYTE);

	return (byte & (1<<1));
}

unsigned read_option_lowlevel(unsigned start, unsigned size, unsigned def)
{
#if CONFIG_USE_OPTION_TABLE
	unsigned byte;
	byte = cmos_read(start/8);
	return (byte >> (start & 7U)) & ((1U << size) - 1U);
#else
	return def;
#endif
}
