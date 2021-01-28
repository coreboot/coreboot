/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <acpi/acpi.h>
#include <arch/ioapic.h>
#include <arch/smp/mpspec.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <version.h>

#include "../qemu-i440fx/fw_cfg.h"
#include "../qemu-i440fx/acpi.h"

void mainboard_fill_fadt(acpi_fadt_t *fadt)
{
	/* Do not advertise SMI even if installed. */
	fadt->smi_cmd = 0;
	fadt->acpi_enable = 0;
	fadt->acpi_disable = 0;
}

unsigned long acpi_fill_madt(unsigned long current)
{
	/* Local APICs */
	current = acpi_create_madt_lapics(current);

	/* IOAPIC */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current,
				2, IO_APIC_ADDR, 0);

	/* INT_SRC_OVR */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
		 current, 0, 0, 2, 0);
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
		 current, 0, 9, 9, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_HIGH);

	return current;
}

unsigned long acpi_fill_mcfg(unsigned long current)
{
	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *)current,
					     CONFIG_MMCONF_BASE_ADDRESS, 0, 0,
					     CONFIG_MMCONF_BUS_NUMBER - 1);
	return current;
}
