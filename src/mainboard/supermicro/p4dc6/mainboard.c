#include <arch/io.h>
#include <part/mainboard.h>
#include <printk.h>
#include <pci.h>
#include <pci_ids.h>
#include <southbridge/intel/82801.h>
#include <arch/smp/mpspec.h>

unsigned long initial_apicid[MAX_CPUS] =
{
	0, 6
};

void mainboard_fixup(void)
{
	ich2_enable_ioapic();
	ich2_enable_serial_irqs();
	ich2_enable_ide(1,1);
	rtc_init();
	ich2_lpc_route_dma(0xff);
	isa_dma_init();
	printk_notice("Please add a mainboard_fixup!\n");
	return;
}

void hard_reset(void)
{
	/* Try rebooting through port 0xcf9 */
	outb((0 <<3)|(1<<2)|(1<<1), 0xcf9);
	return;
}


