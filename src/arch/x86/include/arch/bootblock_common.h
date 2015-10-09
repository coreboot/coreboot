#include <arch/cbfs.h>
#include <cpu/x86/lapic/boot_cpu.c>
#include <pc80/mc146818rtc.h>

#ifdef CONFIG_BOOTBLOCK_RESETS
#include CONFIG_BOOTBLOCK_RESETS
#endif

#ifdef CONFIG_BOOTBLOCK_CPU_INIT
#include CONFIG_BOOTBLOCK_CPU_INIT
#endif
#ifdef CONFIG_BOOTBLOCK_NORTHBRIDGE_INIT
#include CONFIG_BOOTBLOCK_NORTHBRIDGE_INIT
#endif
#ifdef CONFIG_BOOTBLOCK_SOUTHBRIDGE_INIT
#include CONFIG_BOOTBLOCK_SOUTHBRIDGE_INIT
#endif

#ifdef CONFIG_BOOTBLOCK_MAINBOARD_INIT
#include CONFIG_BOOTBLOCK_MAINBOARD_INIT
#else
static void bootblock_mainboard_init(void)
{
#ifdef CONFIG_BOOTBLOCK_NORTHBRIDGE_INIT
	bootblock_northbridge_init();
#endif
#ifdef CONFIG_BOOTBLOCK_SOUTHBRIDGE_INIT
	bootblock_southbridge_init();
#endif
#ifdef CONFIG_BOOTBLOCK_CPU_INIT
	bootblock_cpu_init();
#endif
}
#endif

#if CONFIG_USE_OPTION_TABLE
static void sanitize_cmos(void)
{
	if (cmos_error() || !cmos_chksum_valid() || IS_ENABLED(CONFIG_STATIC_OPTION_TABLE)) {
		unsigned char *cmos_default = (unsigned char*)walkcbfs("cmos.default");
		if (cmos_default) {
			int i;
			cmos_disable_rtc();
			for (i = 14; i < 128; i++) {
				cmos_write_inner(cmos_default[i], i);
			}
			cmos_enable_rtc();
		}
	}
}
#endif

#if CONFIG_CMOS_POST
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
#if CONFIG_CMOS_POST_EXTRA
		cmos_write32(CMOS_POST_BANK_0_EXTRA, 0);
		cmos_write32(CMOS_POST_BANK_1_EXTRA, 0);
#endif
	}

	cmos_write(magic, CMOS_POST_BANK_OFFSET);
}
#endif
