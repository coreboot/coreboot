#include <printk.h>

void
mainboard_fixup(void)
{
}

void
final_mainboard_fixup(void)
{
	void final_southbridge_fixup(void);
	void final_superio_fixup(void);
	void k7_cpufixup(unsigned long ramsizeK);

	extern unsigned long slotsizeM[];

	printk_info("elitegroup k7sem (and similar)...");

	//k7_cpufixup(slotsizeM[0] + slotsizeM[1]);

	final_southbridge_fixup();

#ifndef USE_NEW_SUPERIO_INTERFACE
	final_superio_fixup();
#endif
}
