#include <printk.h>

void
mainboard_fixup(void)
{
}

void
final_mainboard_fixup(void)
{
	void final_southbridge_fixup(void);

	printk_info("SiS 635 (and similar)...");

	final_southbridge_fixup();
}
