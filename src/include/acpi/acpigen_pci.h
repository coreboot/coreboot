/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ACPIGEN_PCI_H
#define ACPIGEN_PCI_H

#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_type.h>

void acpigen_write_ADR_pci_devfn(pci_devfn_t devfn);
void acpigen_write_ADR_pci_device(const struct device *dev);

void acpigen_write_PRT_GSI_entry(unsigned int pci_dev, unsigned int acpi_pin, unsigned int gsi);
void acpigen_write_PRT_source_entry(unsigned int pci_dev, unsigned int acpi_pin,
				    const char *source_path, unsigned int index);

#endif /* ACPIGEN_PCI_H */
