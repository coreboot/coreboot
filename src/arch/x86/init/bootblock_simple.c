#include <bootblock_common.h>

static void main(unsigned long bist)
{
	int bsp_cpu = boot_cpu();

	/* Mainboard-specific early init. */
	init_mainboard(bsp_cpu);

#if CONFIG_USE_OPTION_TABLE
	if (bsp_cpu)
		sanitize_cmos();
#endif

	const char* target1 = "fallback/romstage";
	unsigned long entry;
	entry = findstage(target1);
	if (entry) call(entry, bist);
	asm volatile ("1:\n\thlt\n\tjmp 1b\n\t");
}

