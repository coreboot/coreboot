#include <cpu/x86/lapic/boot_cpu.c>
#include <arch/cbfs.h>

/*
 * Compiler for bootblock is ROMCC and it can only build from a single source
 * file. To overcome this limitation, sconfig utility includes all the required
 * bootblock.c files in a single file named bootblock_devices.h.
 *
 * In this file sconfig utility creates bootblock_mainboard_init(), which
 * calls the individual chips bootblock_xxx_init() functions in the order
 * chips appear in devicetree.cb. If this does not meet requirements, use option
 * HAS_MAINBOARD_BOOTBLOCK and override it in mainboard/x/x/bootblock.c.
 */

#include "bootblock_devices.h"

#if 0	/* non-functional sample of bootblock_devices.h */

#include "cpu/vendor/part/bootblock.c"
#include "northbridge/vendor/part/bootblock.c"
#include "southbridge/vendor/part/bootblock.c"

#if CONFIG_HAS_MAINBOARD_BOOTBLOCK
#include "mainboard/vendor/part/bootblock.c"
#else
static void bootblock_mainboard_init(void)
{
	bootblock_northbridge_init();
	bootblock_southbridge_init();
	bootblock_cpu_init();
}
#endif

#endif	/* non-functional sample of bootblock_devices.h */


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
