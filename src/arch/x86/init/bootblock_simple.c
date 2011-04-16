#include <bootblock_common.h>

static void main(unsigned long bist)
{
	if (boot_cpu()) {
		bootblock_northbridge_init();
		bootblock_southbridge_init();
		bootblock_cpu_init();
	}

#if CONFIG_USE_OPTION_TABLE
	sanitize_cmos();
#endif

	const char* target1 = "fallback/romstage";
	unsigned long entry;
	entry = findstage(target1);
	if (entry) call(entry, bist);
	asm volatile ("1:\n\thlt\n\tjmp 1b\n\t");
}

