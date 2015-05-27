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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _AMD_00730F01_PCI_DEVS_H_
#define _AMD_00730F01_PCI_DEVS_H_

#define BUS0 0

/* Graphics and Display */
#define GFX_DEV 0x1
#define GFX_FUNC 0
#define GFX_DEVID 0x9850
#define GFX_DEVFN PCI_DEVFN(GFX_DEV,GFX_FUNC)

/* Internal Audio controller */
#define ACTL_DEV 0x1
#define ACTL_FUNC 1
#define ACTL_DEVID 0x9840
#define ACTL_DEVFN PCI_DEVFN(ACTL_DEV,ACTL_FUNC)

/* PCIe Ports */
#define NB_PCIE_PORT2_DEV 0x2
#define NB_PCIE_FUNC0	0
#define NB_PCIE_FUNC1	1
#define NB_PCIE_FUNC2	2
#define NB_PCIE_FUNC3	3
#define NB_PCIE_FUNC4	4
#define NB_PCIE_FUNC5	5
#define NB_PCIE_PORT_DEVID 0x1439
#define NB_PCIE_PORT0_DEVFN PCI_DEVFN(NB_PCIE_PORT2_DEV,NB_PCIE_FUNC0)
#define NB_PCIE_PORT1_DEVFN PCI_DEVFN(NB_PCIE_PORT2_DEV,NB_PCIE_FUNC1)
#define NB_PCIE_PORT2_DEVFN PCI_DEVFN(NB_PCIE_PORT2_DEV,NB_PCIE_FUNC2)
#define NB_PCIE_PORT3_DEVFN PCI_DEVFN(NB_PCIE_PORT2_DEV,NB_PCIE_FUNC3)
#define NB_PCIE_PORT4_DEVFN PCI_DEVFN(NB_PCIE_PORT2_DEV,NB_PCIE_FUNC4)
#define NB_PCIE_PORT5_DEVFN PCI_DEVFN(NB_PCIE_PORT2_DEV,NB_PCIE_FUNC5)

#endif /* _AMD_00730F01_PCI_DEVS_H_ */
