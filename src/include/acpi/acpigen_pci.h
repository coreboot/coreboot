/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ACPIGEN_PCI_H
#define ACPIGEN_PCI_H

#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_type.h>

#define PCIE_NATIVE_HOTPLUG_CONTROL  0x01
#define SHPC_NATIVE_HOTPLUG_CONTROL  0x02
#define PCIE_PME_CONTROL             0x04
#define PCIE_AER_CONTROL             0x08
#define PCIE_CAP_STRUCTURE_CONTROL   0x10
#define PCIE_LTR_CONTROL             0x20

void acpigen_write_ADR_pci_devfn(pci_devfn_t devfn);
void acpigen_write_ADR_pci_device(const struct device *dev);

void acpigen_write_PRT_GSI_entry(unsigned int pci_dev, unsigned int acpi_pin, unsigned int gsi);
void acpigen_write_PRT_source_entry(unsigned int pci_dev, unsigned int acpi_pin,
				    const char *source_path, unsigned int index);

void pci_domain_fill_ssdt(const struct device *domain);

void acpigen_write_OSC_pci_domain(const struct device *domain, const bool is_cxl_domain);
uint32_t soc_get_granted_pci_features(const struct device *domain);
uint32_t soc_get_granted_cxl_features(const struct device *domain);

#endif /* ACPIGEN_PCI_H */
