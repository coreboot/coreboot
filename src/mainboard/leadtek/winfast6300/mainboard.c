#include <printk.h>

void
mainboard_fixup(void)
{
    void southbridge_fixup(void);
    southbridge_fixup();
    printk(KERN_INFO "Mainboard fixup done for Winfast 6300 (and similar)\n");
}
