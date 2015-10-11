/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2014 Sage Electronic Engineering, LLC.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef _INTEL_FSP_SANDYBRIDGE_PCI_DEVS_H_
#define _INTEL_FSP_SANDYBRIDGE_PCI_DEVS_H_

#include <device/pci_def.h>

#define BUS0 0

/* NB PCIe PEG slot */
#define NB_PEG_DEV		0x01
#define NB_PEG_FUNC		0
# define NB_PEG_DEVFN	PCI_DEVFN(NB_PEG_DEV, NB_PEG_FUNC)
#define PCIE_CTRL1_FUNC	1
# define PCIE_CTRL1_DEVFN	PCI_DEVFN(NB_PEG_DEV, PCIE_CTRL1_FUNC)
#define PCIE_CTRL2_FUNC	2
# define PCIE_CTRL2_DEVFN	PCI_DEVFN(NB_PEG_DEV, PCIE_CTRL2_FUNC)

/* Onboard Graphics */
#define GFX_DEV		0x02
#define GFX_FUNC	0
# define GFX_DEVFN	PCI_DEVFN(GFX_DEV, GFX_FUNC)

/* NB PCIe slot */
#define NB_PCIE_DEV		0x06
#define NB_PCIE_FUNC	0
# define NB_PCIE_DEVFN	PCI_DEVFN(NB_PCIE_DEV, NB_PCIE_FUNC)

#endif /* _INTEL_FSP_SANDYBRIDGE_PCI_DEVS_H_ */
