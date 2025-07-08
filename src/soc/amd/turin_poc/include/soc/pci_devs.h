/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_TURIN_POC_PCI_DEVS_H
#define AMD_TURIN_POC_PCI_DEVS_H

#include <device/pci_def.h>
#include <amdblocks/pci_devs.h>

/* GNB Root Complex */
#define GNB_DEV			0x0
#define GNB_FUNC		0
#define GNB_DEVFN		PCI_DEVFN(GNB_DEV, GNB_FUNC)
#define SOC_GNB_DEV		_SOC_DEV(GNB_DEV, GNB_FUNC)

/* SMBUS */
#define SMBUS_DEV		0x14
#define SMBUS_FUNC		0
#define SMBUS_DEVFN		PCI_DEVFN(SMBUS_DEV, SMBUS_FUNC)
#define SOC_SMBUS_DEV		_SOC_DEV(SMBUS_DEV, SMBUS_FUNC)

/* LPC BUS */
#define PCU_DEV			0x14
#define LPC_FUNC		3
#define LPC_DEVFN		PCI_DEVFN(PCU_DEV, LPC_FUNC)
#define SOC_LPC_DEV		_SOC_DEV(PCU_DEV, LPC_FUNC)

/* Data Fabric functions */
#define DF_DEV			0x18

#define DF_F0_DEVFN		PCI_DEVFN(DF_DEV, 0)
#define SOC_DF_F0_DEV		_SOC_DEV(DF_DEV, 0)

#define DF_F1_DEVFN		PCI_DEVFN(DF_DEV, 1)
#define SOC_DF_F1_DEV		_SOC_DEV(DF_DEV, 1)

#define DF_F2_DEVFN		PCI_DEVFN(DF_DEV, 2)
#define SOC_DF_F2_DEV		_SOC_DEV(DF_DEV, 2)

#define DF_F3_DEVFN		PCI_DEVFN(DF_DEV, 3)
#define SOC_DF_F3_DEV		_SOC_DEV(DF_DEV, 3)

#define DF_F4_DEVFN		PCI_DEVFN(DF_DEV, 4)
#define SOC_DF_F4_DEV		_SOC_DEV(DF_DEV, 4)

#define DF_F5_DEVFN		PCI_DEVFN(DF_DEV, 5)
#define SOC_DF_F5_DEV		_SOC_DEV(DF_DEV, 5)

#define DF_F6_DEVFN		PCI_DEVFN(DF_DEV, 6)
#define SOC_DF_F6_DEV		_SOC_DEV(DF_DEV, 6)

#define DF_F7_DEVFN		PCI_DEVFN(DF_DEV, 7)
#define SOC_DF_F7_DEV		_SOC_DEV(DF_DEV, 7)

#endif /* AMD_TURIN_POC_PCI_DEVS_H */
