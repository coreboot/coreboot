/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/ioapic.h>
#include <console/console.h>
#include <intelblocks/lpc_lib.h>

void lpc_soc_init(struct device *dev)
{
	printk(BIOS_SPEW, "pch: lpc_init\n");

	if (CONFIG(SOC_INTEL_SKYLAKE_SP)) {
		/* FSP only configures PCH IOAPIC, but it doesn't configure PCI IOAPICs.
		 * Let coreboot handle all IOAPICs and assign unique IDs to each. */
		register_new_ioapic_gsi0(IO_APIC_ADDR);
	}
}
