/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef TEST_ACPI_MOCK_AMD_HARDWARE_H
#define TEST_ACPI_MOCK_AMD_HARDWARE_H

/* Keep the PCI BDF calculation and ACPI serializers real. Replace only the
 * register-access boundary so tests cannot issue native PCI or MMIO accesses.
 */
#define ARCH_I386_PCI_OPS_H
#include <device/pci_type.h>
#include <device/mmio.h>

uint8_t pci_s_read_config8(pci_devfn_t dev, uint16_t reg);
uint16_t pci_s_read_config16(pci_devfn_t dev, uint16_t reg);
uint32_t pci_s_read_config32(pci_devfn_t dev, uint16_t reg);
void pci_s_write_config8(pci_devfn_t dev, uint16_t reg, uint8_t value);
void pci_s_write_config16(pci_devfn_t dev, uint16_t reg, uint16_t value);
void pci_s_write_config32(pci_devfn_t dev, uint16_t reg, uint32_t value);

uint32_t acpi_test_read32(const volatile void *addr);
uint64_t acpi_test_read64(const volatile void *addr);

#define read32 acpi_test_read32
#define read64 acpi_test_read64

#endif
