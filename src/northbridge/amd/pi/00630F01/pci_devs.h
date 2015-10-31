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

#ifndef _AMD_00630F01_PCI_DEVS_H_
#define _AMD_00630F01_PCI_DEVS_H_

#define BUS0 0

/* Graphics and Display */
#define GFX_DEV 0x1
#define GFX_FUNC 0
#define GFX_DEVID 0x1304
#define GFX_DEVFN PCI_DEVFN(GFX_DEV,GFX_FUNC)
#define PIRQ_GFX	FCH_INT_TABLE_SIZE
                            /* Integrated graphics device, must be after the
                             * last C00/C01 entry
                             */

/* Internal Audio controller */
#define ACTL_DEV 0x1
#define ACTL_FUNC 1
#define ACTL_DEVID 0x1308
#define ACTL_DEVFN PCI_DEVFN(ACTL_DEV,ACTL_FUNC)
#define PIRQ_ACTL	FCH_INT_TABLE_SIZE+1
                            /* Integrated HDMI audio device, must be after the
                             * last C00/C01 entry
                             */

/* PCIe Ports */
#define NB_GFX_PCIE_PORTS_DEV 0x2
#define NB_GPP_PCIE_PORTS_DEV 0x3
#define NB_PCIE_PORT1_DEVFN PCI_DEVFN(NB_GFX_PCIE_PORTS_DEV,0x01)
#define NB_PCIE_PORT2_DEVFN PCI_DEVFN(NB_GFX_PCIE_PORTS_DEV,0x02)
#define NB_PCIE_PORT3_DEVFN PCI_DEVFN(NB_GPP_PCIE_PORTS_DEV,0x01)
#define NB_PCIE_PORT4_DEVFN PCI_DEVFN(NB_GPP_PCIE_PORTS_DEV,0x02)
#define NB_PCIE_PORT5_DEVFN PCI_DEVFN(NB_GPP_PCIE_PORTS_DEV,0x03)
#define NB_PCIE_PORT6_DEVFN PCI_DEVFN(NB_GPP_PCIE_PORTS_DEV,0x04)
#define NB_PCIE_PORT7_DEVFN PCI_DEVFN(NB_GPP_PCIE_PORTS_DEV,0x05)

#endif /* _AMD_00630F01_PCI_DEVS_H_ */
