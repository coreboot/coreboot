/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef SOC_INTEL_COMMON_BLOCK_XDCI_H
#define SOC_INTEL_COMMON_BLOCK_XDCI_H

void soc_xdci_init(struct device *dev);
int xdci_can_enable(void);

#endif	/* SOC_INTEL_COMMON_BLOCK_XDCI_H */
