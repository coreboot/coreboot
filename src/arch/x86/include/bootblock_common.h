#define __PRE_RAM__
#if CONFIG_LOGICAL_CPUS && \
 (defined(CONFIG_BOOTBLOCK_NORTHBRIDGE_INIT) || defined(CONFIG_BOOTBLOCK_SOUTHBRIDGE_INIT))
#include <cpu/x86/lapic/boot_cpu.c>
#else
#define boot_cpu(x) 1
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

static void *walkcbfs(char *target)
{
	void *entry;
	asm volatile (
		"mov $1f, %%esp\n\t"
		"jmp walkcbfs_asm\n\t"
		"1:\n\t" : "=a" (entry) : "S" (target) : "ebx", "ecx", "edi", "esp");
	return entry;
}

/* just enough to support findstage. copied because the original version doesn't easily pass through romcc */
struct cbfs_stage {
	unsigned long compression;
	unsigned long entry; // this is really 64bit, but properly endianized
};

static unsigned long findstage(char* target)
{
	return ((struct cbfs_stage*)walkcbfs(target))->entry;
}

static void call(unsigned long addr, unsigned long bist)
{
	asm volatile ("jmp *%0\n\t" : : "r" (addr), "a" (bist));
}

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
