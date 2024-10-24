/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <intelblocks/itss.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <soc/pcr_ids.h>

#include <acpi/acpigen.h>
#include <acpi/acpigen_pci.h>

void lpc_soc_fill_ssdt(const struct device *dev)
{
	const char *scope = acpi_device_scope(dev);
	const char *name = acpi_device_name(dev);

	if (!scope || !name) {
		printk(BIOS_ERR, "%s: Missing ACPI path/scope\n", dev_path(dev));
		return;
	}

	/* Device */
	acpigen_write_scope(scope);
	acpigen_write_device(name);

	printk(BIOS_DEBUG, "%s.%s: %s\n", scope, name, dev_path(dev));
	acpigen_write_ADR_pci_device(dev);

	acpigen_write_device_end(); /* Device */
	acpigen_write_scope_end(); /* Scope */
}

void lpc_soc_init(struct device *dev)
{
	printk(BIOS_SPEW, "pch: lpc_init\n");

	/* Program irq pin/line for PCI devices by PCH convention */
	pch_pirq_init();

	/* Explicitly set polarity low for PIRQA to PIRQH */
	for (int i = 0; i < PIRQ_COUNT; i++) {
		itss_set_irq_polarity(pcr_read8(PID_ITSS, PCR_ITSS_PIRQA_ROUT + i), 1);
	}
}
