/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_P2SBLIB_H
#define SOC_INTEL_COMMON_BLOCK_P2SBLIB_H

#include <stddef.h>
#include <stdint.h>

/* P2SB generic configuration register */
#define P2SBC		0xe0
#define P2SBC_HIDE_BIT	(1 << 0)

void p2sb_dev_enable_bar(pci_devfn_t dev, uint64_t bar);
bool p2sb_dev_is_hidden(pci_devfn_t dev);
void p2sb_dev_unhide(pci_devfn_t dev);
void p2sb_dev_hide(pci_devfn_t dev);
uint32_t p2sb_dev_sbi_read(pci_devfn_t dev, uint8_t pid, uint16_t reg);
void p2sb_dev_sbi_write(pci_devfn_t dev, uint8_t pid, uint16_t reg, uint32_t val);

#endif	/* SOC_INTEL_COMMON_BLOCK_P2SBLIB_H */
