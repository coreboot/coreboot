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

	printk_info("elitegroup k7sem (and similar)...");

#if 0
	// THIS WORKS in the sense of turning it off. 
	// but it happens too late in the game.
	printk_info("disabling sis 900 ethernet, old val 0x%x\n", 
		    pcibios_read_config_byte(0, 0x8, 0x7c));

        pcibios_write_config_byte(0, 0x8, 0x7c, 0x0f);

	printk_info("disabling sis 900 ethernet, new val 0x%x\n", 
		    pcibios_read_config_byte(0, 0x8, 0x7c));
#endif

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
