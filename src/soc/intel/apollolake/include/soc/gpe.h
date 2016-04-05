/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Intel Corporation.
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

#ifndef _SOC_GPE_H_
#define _SOC_GPE_H_

/* bit position in GPE0a_STS register */
#define GPE0A_PCIE_SCI_STS		0
#define GPE0A_SWGPE_STS			2
#define GPE0A_PCIE_WAKE0_STS		3
#define GPE0A_PUNIT_SCI_STS		4
#define GPE0A_PCIE_WAKE1_STS		6
#define GPE0A_PCIE_WAKE2_STS		7
#define GPE0A_PCIE_WAKE3_STS		8
#define GPE0A_PCIE_GPE_STS		9
#define GPE0A_BATLOW_STS		10
#define GPE0A_CSE_PME_STS		11
#define GPE0A_XDCI_PME_STS		12
#define GPE0A_XHCI_PME_STS		13
#define GPE0A_AVS_PME_STS		14
#define GPE0A_GPIO_TIER1_SCI_STS	15
#define GPE0A_SMB_WAK_STS		16
#define GPE0A_SATA_PME_STS		17

#endif
