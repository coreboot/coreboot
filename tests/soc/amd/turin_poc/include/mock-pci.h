/* SPDX-License-Identifier: GPL-2.0-only */

/* Replace only the hardware PCI access boundary. The SMN and IOHC code under
 * test still selects the BDF, issues the index/data transactions and masks BARs.
 */
#ifndef TEST_TURIN_MOCK_PCI_H
#define TEST_TURIN_MOCK_PCI_H

#define ARCH_I386_PCI_OPS_H
#include <device/pci_type.h>

uint8_t pci_s_read_config8(pci_devfn_t dev, uint16_t reg);
uint16_t pci_s_read_config16(pci_devfn_t dev, uint16_t reg);
uint32_t pci_s_read_config32(pci_devfn_t dev, uint16_t reg);
void pci_s_write_config8(pci_devfn_t dev, uint16_t reg, uint8_t value);
void pci_s_write_config16(pci_devfn_t dev, uint16_t reg, uint16_t value);
void pci_s_write_config32(pci_devfn_t dev, uint16_t reg, uint32_t value);

#endif
