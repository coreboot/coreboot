#include <printk.h>
#include <pci.h>

#include <cpu/p5/io.h>
#include <cpu/p5/cpuid.h>
#include <arch/pirq_routing.h>
#include <arch/i386_subr.h>
#include <part/nvram.h>
#include <subr.h>

void mainboard_fixup()
{
	nvram_on();

	//intel_display_cpuid();
	display_cpuid();
	//intel_mtrr_check();
	mtrr_check();

	//intel_zero_irq_settings();
	pci_zero_irq_settings();
	//intel_check_irq_routing_table();
	check_pirq_routing_table();
	//intel_interrupts_on();
	interrupts_on();
}
