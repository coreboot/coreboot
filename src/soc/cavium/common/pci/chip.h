/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018-present Facebook, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
