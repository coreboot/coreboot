/*
 * This file is part of the coreboot project.
 *
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
 */

#ifndef _AMD_FAM16KB_PCI_DEVS_H_
#define _AMD_FAM16KB_PCI_DEVS_H_

#define BUS0 0

/* Graphics and Display */
#define GFX_DEV 0x1
#define GFX_FUNC 0
#define GFX_DEVFN PCI_DEVFN(GFX_DEV,GFX_FUNC)

/* Internal Audio Controller */
#define ACTL_FUNC	1
#define ACTL_DEVFN	PCI_DEVFN(GFX_DEV,ACTL_FUNC)

/* PCIe Ports */
#define NB_PCIE_PORT1_FUNC 0x1
#define NB_PCIE_PORT2_FUNC 0x2
#define NB_PCIE_PORT3_FUNC 0x3
#define NB_PCIE_PORT4_FUNC 0x4
#define NB_PCIE_PORT5_FUNC 0x5
#define NB_PCIE_DEV 2
#define NB_PCIE_PORT_DEVID 0x1439
#define NB_PCIE_PORT1_DEVFN PCI_DEVFN(NB_PCIE_DEV,NB_PCIE_PORT1_FUNC)
#define NB_PCIE_PORT2_DEVFN PCI_DEVFN(NB_PCIE_DEV,NB_PCIE_PORT2_FUNC)
#define NB_PCIE_PORT3_DEVFN PCI_DEVFN(NB_PCIE_DEV,NB_PCIE_PORT3_FUNC)
#define NB_PCIE_PORT4_DEVFN PCI_DEVFN(NB_PCIE_DEV,NB_PCIE_PORT4_FUNC)
#define NB_PCIE_PORT5_DEVFN PCI_DEVFN(NB_PCIE_DEV,NB_PCIE_PORT5_FUNC)

#endif /* _AMD_FAM16KB_PCI_DEVS_H_ */
