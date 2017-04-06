/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
 * Copyright (C) 2017 Siemens AG
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

#ifndef _SOC_PCI_DEVS_H_
#define _SOC_PCI_DEVS_H_

#include <device/pci_def.h>

#define BUS0 0

#define SOC_DEV                 0
#define SOC_FUNC                0
#define SOC_DEVID               0x2F00
#define SOC_DEVID_ES2           0x6F00
#define SOC_DEV_FUNC PCI_DEVFN(SOC_DEV, SOC_FUNC)

#define VTD_DEV                 5
#define VTD_FUNC                0
#define VTD_DEVID               0x6f28
#define VTD_DEV_FUNC PCI_DEVFN(VTD_DEV, VTD_FUNC)

#define LPC_DEV                 31
#define LPC_FUNC                0
#define LPC_DEVID               0x8C42
#define LPC_DEVID_ES2           0x8C54
#define LPC_DEV_FUNC PCI_DEVFN(LPC_DEV, LPC_FUNC)

#define SATA_DEV                31
#define SATA_FUNC               2
#define AHCI_DEVID              0x8C02
#define SATA_DEV_FUNC PCI_DEVFN(SATA_DEV, SATA_FUNC)

#define SMBUS_DEV                31
#define SMBUS_FUNC               3
#define SMBUS_DEVID              0x8C22
#define SMBUS_DEV_FUNC PCI_DEVFN(SMBUS_DEV, SMBUS_FUNC)

#define SATA2_DEV               31
#define SATA2_FUNC              5
#define SATA2_DEV_FUNC PCI_DEVFN(SATA2_DEV, SATA2_FUNC)

#define EHCI1_DEV               29
#define EHCI1_FUNC              0
#define EHCI1_DEVID             0x8C26
#define EHCI1_DEV_FUNC PCI_DEVFN(EHCI_DEV1, EHCI_FUNC1)

#define EHCI2_DEV               26
#define EHCI2_FUNC              0
#define EHCI2_DEVID             0x8C2D
#define EHCI2_DEV_FUNC PCI_DEVFN(EHCI_DEV2, EHCI_FUNC2)

#define XHCI_DEV                20
#define XHCI_FUNC               0
#define XHCI_DEVID              0x8C31
#define XHCI_FUS_REG            0xE0
#define XHCI_FUNC_DISABLE       (1 << 0)
#define XHCI_USB2PR_REG         0xD0
#define XHCI_DEV_FUNC PCI_DEVFN(XHCI_DEV, XHCI_FUNC)

#define GBE_DEV                 25
#define GBE_FUNC                0
#define GBE_DEVID               0x8C33
#define GBE_DEV_FUNC PCI_DEVFN(GBE_DEV, GBE_FUNC)

#define ME_DEV                  22
#define ME_FUNC                 0
#define ME_DEVID                0x8C3A
#define ME_DEV_FUNC PCI_DEVFN(ME_DEV, ME_FUNC)

#define HDA_DEV                 27
#define HDA_FUNC                0
#define HDA_DEVID               0x8C20
#define HDA_DEV_FUNC PCI_DEVFN(HDA_DEV, HDA_FUNC)

#define PCIE_DEV                28
#define PCIE_PORT1_DEV          PCIE_DEV
#define PCIE_PORT1_FUNC         0
#define PCIE_PORT1_DEVID        0x8C10
#define PCIE_PORT2_DEV          PCIE_DEV
#define PCIE_PORT2_FUNC         1
#define PCIE_PORT2_DEVID        0x8C12
#define PCIE_PORT3_DEV          PCIE_DEV
#define PCIE_PORT3_FUNC         2
#define PCIE_PORT3_DEVID        0x8C14
#define PCIE_PORT4_DEV          PCIE_DEV
#define PCIE_PORT4_FUNC         3
#define PCIE_PORT4_DEVID        0x8C16
#define PCIE_PORT5_DEV          PCIE_DEV
#define PCIE_PORT5_FUNC         4
#define PCIE_PORT5_DEVID        0x8C18
#define PCIE_PORT6_DEV          PCIE_DEV
#define PCIE_PORT6_FUNC         5
#define PCIE_PORT6_DEVID        0x8C1A
#define PCIE_PORT7_DEV          PCIE_DEV
#define PCIE_PORT7_FUNC         6
#define PCIE_PORT7_DEVID        0x8C1C
#define PCIE_PORT8_DEV          PCIE_DEV
#define PCIE_PORT8_FUNC         7
#define PCIE_PORT8_DEVID        0x8C1E
#define PCIE_PORT1_DEV_FUNC PCI_DEVFN(PCIE_DEV, PCIE_PORT1_FUNC)
#define PCIE_PORT2_DEV_FUNC PCI_DEVFN(PCIE_DEV, PCIE_PORT2_FUNC)
#define PCIE_PORT3_DEV_FUNC PCI_DEVFN(PCIE_DEV, PCIE_PORT3_FUNC)
#define PCIE_PORT4_DEV_FUNC PCI_DEVFN(PCIE_DEV, PCIE_PORT4_FUNC)
#define PCIE_PORT5_DEV_FUNC PCI_DEVFN(PCIE_DEV, PCIE_PORT5_FUNC)
#define PCIE_PORT6_DEV_FUNC PCI_DEVFN(PCIE_DEV, PCIE_PORT6_FUNC)
#define PCIE_PORT7_DEV_FUNC PCI_DEVFN(PCIE_DEV, PCIE_PORT7_FUNC)
#define PCIE_PORT8_DEV_FUNC PCI_DEVFN(PCIE_DEV, PCIE_PORT8_FUNC)

/* The SMM device is located on bus 0xff (QPI) */
#define QPI_BUS			0xff
#define SMM_DEV			0x10
#define SMM_FUNC		0x06
#define SMM_DEV_FUNC		PCI_DEVFN(SMM_DEV, SMM_FUNC)

#endif /* _SOC_PCI_DEVS_H_ */
