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

	printk_info("DELL 350\n");

	final_southbridge_fixup();
#ifndef USE_NEW_SUPERIO_INTERFACE
	final_superio_fixup();
#endif
}
