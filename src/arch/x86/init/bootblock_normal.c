#include <bootblock_common.h>
#include <pc80/mc146818rtc.h>

static void main(unsigned long bist)
{
	if (boot_cpu()) {
		bootblock_northbridge_init();
		bootblock_southbridge_init();
	}

	unsigned long entry;
	if (do_normal_boot())
		entry = findstage("normal/romstage");
	else
		entry = findstage("fallback/romstage");

	if (entry) call(entry, bist);

	/* run fallback if normal can't be found */
	entry = findstage("fallback/romstage");
	if (entry) call(entry, bist);

	/* duh. we're stuck */
	asm volatile ("1:\n\thlt\n\tjmp 1b\n\t");
}

