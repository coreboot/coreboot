#if 0
#include <printk.h>
#endif

void
mainboard_fixup(void)
{
}

void
final_mainboard_fixup(void)
{
#if 0
//	void final_southbridge_fixup(void);
//	void final_superio_fixup(void);

	printk_info("AMD Solo initializing...");

//	final_southbridge_fixup();

//#ifndef USE_NEW_SUPERIO_INTERFACE
//final_superio_fixup();
//#endif
#endif
}
