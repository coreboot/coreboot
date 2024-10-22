/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <intelblocks/lpc_lib.h>

void lpc_soc_init(struct device *dev)
{
	printk(BIOS_SPEW, "pch: lpc_init\n");

	/* FSP configures IOAPIC and PCHInterrupt Config */
	/* Should read back the ID to fill ACPI IOAPIC accordingly. */
}
