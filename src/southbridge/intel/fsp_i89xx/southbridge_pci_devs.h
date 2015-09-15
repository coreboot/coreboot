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

#ifndef _INTEL_FSP_BD82X6X_PCI_DEVS_H_
#define _INTEL_FSP_BD82X6X_PCI_DEVS_H_

#include <device/pci_def.h>

#define BUS0 0

/* Management Engine Interface */
#define MEI_DEV		0x16
#define MEI1_FUNC	0x0
# define MEI1_DEVFN	PCI_DEVFN(MEI_DEV, MEI1_FUNC)

#define MEI2_FUNC	0x1
# define MEI2_DEVFN	PCI_DEVFN(MEI_DEV, MEI2_FUNC)

/* MEI - IDE Redirection */
#define IDE_DEV		0x16
#define IDE_FUNC	2
# define IDE_DEVFN	PCI_DEVFN(IDE_DEV, IDE_FUNC)

/* MEI - Keyboard / Text Redirection */
#define KT_DEV		0x16
#define KT_FUNC		0x3
#define KT_DEVFN	PCI_DEVFN(KT_DEV, KT_FUNC)

/* PCIe Ports */
#define SB_PCIE_DEV				0x1C
#define SB_PCIE_PORT1_FUNC		0
# define SB_PCIE_PORT1_DEVFN	PCI_DEVFN(SB_PCIE_DEV, SB_PCIE_PORT1_FUNC)

#define SB_PCIE_PORT2_FUNC		1
# define SB_PCIE_PORT2_DEVFN	PCI_DEVFN(SB_PCIE_DEV, SB_PCIE_PORT2_FUNC)

#define SB_PCIE_PORT3_FUNC		2
# define SB_PCIE_PORT3_DEVFN	PCI_DEVFN(SB_PCIE_DEV, SB_PCIE_PORT3_FUNC)

#define SB_PCIE_PORT4_FUNC		3
# define SB_PCIE_PORT4_DEVFN	PCI_DEVFN(SB_PCIE_DEV, SB_PCIE_PORT4_FUNC)

/* EHCI */
#define EHCI1_DEV		0x1D
#define EHCI1_FUNC		0
#define EHCI1_DEVID		0x2334
# define EHCI1_DEVFN	PCI_DEVFN(EHCI1_DEV, EHCI1_FUNC)

/* Platform Controller Hub */
#define PCH_DEV			0x1F

/* LPC */
#define LPC_DEV			PCH_DEV
#define LPC_FUNC		0
# define LPC_DEVFN		PCI_DEVFN(LPC_DEV, LPC_FUNC)

/* SATA */
#define SATA_DEV		PCH_DEV
#define SATA1_FUNC		2
# define SATA1_DEVFN	PCI_DEVFN(SATA_DEV, SATA1_FUNC)

/* SMBUS */
#define SMBUS_DEV		PCH_DEV
#define SMBUS_FUNC		3
# define SMBUS_DEVFN	PCI_DEVFN(SMBUS_DEV, SMBUS_FUNC)

#define SATA2_FUNC		5
# define SATA2_DEVFN	PCI_DEVFN(SATA_DEV, SATA2_FUNC)

/* Thermal Sensor */
#define TS_DEV			PCH_DEV
#define TS_FUNC			6
# define TS_DEVFN		PCI_DEVFN(TS_DEV, TS_FUNC)

/* Watchdog Timer */
#define WDT_DEV			PCH_DEV
#define WDT TS_FUNC		7
# define WDT_DEVFN		PCI_DEVFN(WDT_DEV, WDT_FUNC)

#endif /* _INTEL_FSP_BD82X6X_PCI_DEVS_H_ */
