#include <arch/io.h>
#include <part/mainboard.h>
#include <printk.h>
#include <pci.h>
#include <pci_ids.h>
#include <southbridge/intel/82801.h>
#include <arch/smp/mpspec.h>
#include <pc80/isa_dma.h>
#include <cpu/i786/multiplier.h>
#include <superio/w83627hf.h>

unsigned long initial_apicid[MAX_CPUS] =
{
	0, 6
};


void mainboard_fixup(void)
{
	ich2_enable_ioapic();
	ich2_enable_serial_irqs();
	ich2_enable_ide(1,1);
	ich2_rtc_init();
	ich2_lpc_route_dma(0xff);
	isa_dma_init();
#if 1
	/* FIXME don't hard code these */
	ich2_set_cpu_multiplier(XEON_X17);
#endif
	ich2_power_after_power_fail(1);
	w832627hf_power_after_power_fail(POWER_ON);
	printk_notice("Please add a mainboard_fixup!\n");
	return;
}

void hard_reset(void)
{
	ich2_hard_reset();
}

