#include <bootblock_common.h>
#include "bootstrap_dcd.h"

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

	unsigned long entry;
	int boot_mode = 0;

#if CONFIG_TTYS0_DCD_HOOK
	/* DCD high selects normal */
	boot_mode = early_dcd_hook();
#endif
	if (boot_mode)
		entry = findstage("normal/romstage");
	else
		entry = findstage("fallback/romstage");

	if (entry) call(entry, bist);

	/* duh. we're stuck */
	asm volatile ("1:\n\thlt\n\tjmp 1b\n\t");
}

