#include <cpu/x86/lapic/boot_cpu.c>
#include <arch/cbfs.h>
#include "bootblock_devices.h"

#if CONFIG_USE_OPTION_TABLE
#include <pc80/mc146818rtc.h>

static void sanitize_cmos(void)
{
	if (cmos_error() || !cmos_chksum_valid()) {
		unsigned char *cmos_default = (unsigned char*)walkcbfs("cmos.default");
		if (cmos_default) {
			int i;
			for (i = 14; i < 128; i++) {
				cmos_write(cmos_default[i], i);
			}
		}
	}
}
#endif

#if CONFIG_CMOS_POST
#include <pc80/mc146818rtc.h>

static void cmos_post_init(void)
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
	}

	cmos_write(magic, CMOS_POST_BANK_OFFSET);
}
#endif
