#include <printk.h>

void
mainboard_fixup(void)
{
}

void
final_mainboard_fixup(void)
{
    void final_southbridge_fixup(void);
    final_southbridge_fixup();
    printk(KERN_INFO "Final mainboard fixup done for Winfast 6300 (and similar)\n");
}
