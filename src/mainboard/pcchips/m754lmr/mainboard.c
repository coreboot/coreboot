#include <printk.h>
#include <pci.h>

#include <cpu/p5/io.h>

void mainboard_fixup()
{

	printk_info("mainboard_fixup()\n");
}

void final_mainboard_fixup()
{
    void final_northbridge_fixup(void);

    final_northbridge_fixup();
}
