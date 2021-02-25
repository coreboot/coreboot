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
