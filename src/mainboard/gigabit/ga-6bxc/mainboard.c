#include <printk.h>
#include <pci.h>

#include <cpu/p5/io.h>
#include <subr.h>

void mainboard_fixup()
{
	nvram_on();

	intel_display_cpuid();
	intel_mtrr_check();

	intel_zero_irq_settings();
	intel_check_irq_routing_table();
	intel_interrupts_on();
}
