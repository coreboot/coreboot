#include <printk.h>

void
mainboard_fixup(void)
{
}

void
final_mainboard_fixup(void)
{
	void fixk7msr_2dimms(unsigned long dimm1sizeK, 
		unsigned long dimm2sizeK);
	void final_southbridge_fixup(void);
	void final_superio_fixup(void);
	extern unsigned long slotsizeM[];
	printk_info("PCCHIPS M810LMR (and similar)...");

	// we need to fix up the K7 MSRs. 
	// to do this, we get the DIMM sizes in slot1 and 2, and 
	// call fixk7msr_2dimms
	// this is all very mainboard-specific ...

	printk_err("Fixing mainboard for dimms %dK + %dK\n", 
		slotsizeM[0], slotsizeM[1]);
	fixk7msr_2dimms(slotsizeM[0], slotsizeM[1]);
	final_southbridge_fixup();
#ifndef USE_NEW_SUPERIO_INTERFACE
	final_superio_fixup();
#endif
}
