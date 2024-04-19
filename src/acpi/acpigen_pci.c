/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpigen_pci.h>
#include <assert.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_type.h>
#include <types.h>

void acpigen_write_ADR_pci_devfn(pci_devfn_t devfn)
{
	/*
	 * _ADR for PCI Bus is encoded as follows:
	 * [63:32] - unused
	 * [31:16] - device #
	 * [15:0]  - function #
	 */
	acpigen_write_ADR(PCI_SLOT(devfn) << 16 | PCI_FUNC(devfn));
}

void acpigen_write_ADR_pci_device(const struct device *dev)
{
	assert(dev->path.type == DEVICE_PATH_PCI);
	acpigen_write_ADR_pci_devfn(dev->path.pci.devfn);
}

void acpigen_write_PRT_GSI_entry(unsigned int pci_dev, unsigned int acpi_pin, unsigned int gsi)
{
	acpigen_write_package(4);
	acpigen_write_dword((pci_dev << 16) | 0xffff);
	acpigen_write_byte(acpi_pin);

	/* Source */
	acpigen_write_byte(0);

	/* Source Index */
	acpigen_write_dword(gsi);

	acpigen_pop_len(); /* Package */
}

void acpigen_write_PRT_source_entry(unsigned int pci_dev, unsigned int acpi_pin,
				    const char *source_path, unsigned int index)
{
	acpigen_write_package(4);
	acpigen_write_dword((pci_dev << 16) | 0xffff);
	acpigen_write_byte(acpi_pin);

	/* Source */
	acpigen_emit_namestring(source_path);

	/* Source Index */
	acpigen_write_dword(index);

	acpigen_pop_len(); /* Package */
}
