#include <part/mainboard.h>
#include <printk.h>

void mainboard_fixup(void)
{
	ich2_enable_ioapic();
	ich2_enable_serial_irqs();
	printk_notice("Please add a mainboard_fixup!\n");
	return;
}
