#include <pc80/mc146818rtc.h>
#include <fallback.h>
#include <cbfs.h>
#if CONFIG_USE_OPTION_TABLE
#include "option_table.h"
#endif

#ifndef CONFIG_MAX_REBOOT_CNT
#error "CONFIG_MAX_REBOOT_CNT not defined"
#endif
#if  CONFIG_MAX_REBOOT_CNT > 15
#error "CONFIG_MAX_REBOOT_CNT too high"
#endif

#include <console/loglevel.h>

int do_printk(int msg_level, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
#define printk_warning(fmt, arg...) do_printk(BIOS_WARNING ,fmt, ##arg)
#define printk_debug(fmt, arg...) do_printk(BIOS_DEBUG ,fmt, ##arg)

static int cmos_error(void)
{
	unsigned char reg_d;
	/* See if the cmos error condition has been flagged */
	reg_d = cmos_read(RTC_REG_D);
	printk_debug("CMOS_REG_D(VRT): %x\n", reg_d & RTC_VRT);
	return (reg_d & RTC_VRT) == 0;
}

static int cmos_chksum_valid(void)
{
#if CONFIG_USE_OPTION_TABLE
	unsigned char addr;
	unsigned long sum, old_sum;
	sum = 0;
	/* Comput the cmos checksum */
	for(addr = LB_CKS_RANGE_START; addr <= LB_CKS_RANGE_END; addr++) {
		sum += cmos_read(addr);
	}
	sum = (sum & 0xffff) ^ 0xffff;

	/* Read the stored checksum */
	old_sum = cmos_read(LB_CKS_LOC) << 8;
	old_sum |=  cmos_read(LB_CKS_LOC+1);

	printk_debug("CMOS checksum: old = %lx, new=%lx\n", old_sum, sum);
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
	char *cmos_default = cbfs_find_file("cmos.default", 0xaa);
	unsigned char byte;
	int i;

	if (cmos_error() || !cmos_chksum_valid()) {
		if (cmos_default) {
			printk_warning("WARNING - CMOS CORRUPTED. RESTORING DEFAULTS.\n");
			/* First 14 bytes are reserved for
			   RTC and ignored by nvramtool, too.
			   Only 128 bytes: 128+ requires cmos configuration and
			   contains only suspend-to-ram data, which isn't part
			   of the recovery procedure. */
			for (i = 14; i < 128; i++) {
				cmos_write(cmos_default[i], i);
			}
			/* Now reboot to run with default cmos. */
			outb(0x06, 0xcf9);
			for (;;) asm("hlt"); /* Wait for reset! */
		}

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

unsigned read_option(unsigned start, unsigned size, unsigned def)
{
#if CONFIG_USE_OPTION_TABLE
	unsigned byte;
	byte = cmos_read(start/8);
	return (byte >> (start & 7U)) & ((1U << size) - 1U);
#else
	return def;
#endif
}
