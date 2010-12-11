#include <bootblock_common.h>

static void main(unsigned long bist)
{
	if (boot_cpu()) {
		bootblock_northbridge_init();
		bootblock_southbridge_init();
	}
	const char* target1 = "fallback/romstage";
	unsigned long entry;
	entry = findstage(target1);
	if (entry) call(entry, bist);
	asm volatile ("1:\n\thlt\n\tjmp 1b\n\t");
}

