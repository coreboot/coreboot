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

static unsigned long findstage(char* target)
{
	unsigned long entry;
	asm volatile (
		"mov $1f, %%esp\n\t"
		"jmp walkcbfs\n\t"
		"1:\n\t" : "=a" (entry) : "S" (target) : "ebx", "ecx", "edi", "esp");
	return entry;
}

static void call(unsigned long addr, unsigned long bist)
{
	asm volatile ("jmp *%0\n\t" : : "r" (addr), "a" (bist));
}
