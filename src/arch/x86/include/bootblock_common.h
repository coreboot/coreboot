#if CONFIG_LOGICAL_CPUS && \
 (defined(CONFIG_BOOTBLOCK_NORTHBRIDGE_INIT) || defined(CONFIG_BOOTBLOCK_SOUTHBRIDGE_INIT))
#include <cpu/x86/lapic/boot_cpu.c>
#else
#define boot_cpu(x) 1
#endif

#ifdef CONFIG_BOOTBLOCK_CPU_INIT
#include CONFIG_BOOTBLOCK_CPU_INIT
#else
static void bootblock_cpu_init(void) { }
#endif
#ifdef CONFIG_BOOTBLOCK_NORTHBRIDGE_INIT
#include CONFIG_BOOTBLOCK_NORTHBRIDGE_INIT
#else
static void bootblock_northbridge_init(void) { }
#endif
#ifdef CONFIG_BOOTBLOCK_SOUTHBRIDGE_INIT
#include CONFIG_BOOTBLOCK_SOUTHBRIDGE_INIT
#else
static void bootblock_southbridge_init(void) { }
#endif

#include <arch/cbfs.h>

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
