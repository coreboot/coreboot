/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_CAVIUM_COMMON_PCI_CHIP_H
#define __SOC_CAVIUM_COMMON_PCI_CHIP_H

struct soc_cavium_common_pci_config {
	/**
	 * Mark the PCI device as secure.
	 * It will be visible from EL3, but hidden in EL2-0.
	 */
	u8 secure;
};

#endif /* __SOC_CAVIUM_COMMON_PCI_CHIP_H */
