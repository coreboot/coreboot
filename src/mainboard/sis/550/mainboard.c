#include <printk.h>

void
mainboard_fixup(void)
{
}

void
final_mainboard_fixup(void)
{
    void final_southbridge_fixup(void);

    printk(KERN_INFO 
	   "SiS 550 (and similar)...");

    final_southbridge_fixup();
}
