#include <bootblock_common.h>

static void main(unsigned long bist)
{
	if (boot_cpu()) {
		bootblock_mainboard_init();

#if CONFIG_USE_OPTION_TABLE
		sanitize_cmos();
#endif
#if CONFIG_CMOS_POST
		cmos_post_init();
#endif
	}

	const char* target1 = "fallback/romstage";
	unsigned long entry;
	entry = findstage(target1);
	if (entry) call(entry, bist);
	asm volatile ("1:\n\thlt\n\tjmp 1b\n\t");
}

