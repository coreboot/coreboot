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
#include "q35.h"

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
	struct device *dev;
	u32 reg;

	dev = dev_find_device(0x8086, 0x29c0, 0);
	if (!dev)
		return current;

	reg = pci_read_config32(dev, D0F0_PCIEXBAR_LO);
	if ((reg & 0x07) != 0x01)  /* require enabled + 256MB size */
		return current;

	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *) current,
					     reg & 0xf0000000, 0x0, 0x0, 255);
	return current;
}
