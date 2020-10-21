/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DENVERTON_NS_PCI_DEVS_H_
#define _DENVERTON_NS_PCI_DEVS_H_

/* All these devices live on bus 0 with the associated device and function */

#define _PCH_DEVFN(slot, func) PCI_DEVFN(PCH_DEV_SLOT_##slot, func)

#if !defined(__SIMPLE_DEVICE__)
#include <device/device.h>
#include <device/pci_def.h>
#define _PCH_DEV(slot, func) pcidev_path_on_root_debug(_PCH_DEVFN(slot, func), __func__)
#else
#define _PCH_DEV(slot, func) PCI_DEV(0, PCH_DEV_SLOT_##slot, func)
#endif

/* SoC transaction router */
#define SA_DEV 0x0
#define SA_FUNC 0
#define SOC_DEV SA_DEV
#define SOC_FUNC SA_FUNC

/* RAS */
#define RAS_DEV 0x4
#define RAS_FUNC 0

/* Root Complex Event Collector */
#define RCEC_DEV 0x5
#define RCEC_FUNC 0

/* Virtual Root Port 2 */
#define VRP2_DEV 0x6
#define VRP2_FUNC 0

/* PCIe Root Ports */
#define PCIE_DEV 0x09
#define MAX_PCIE_PORT 0x8
#define PCIE_PORT1_DEV 0x09
#define PCIE_PORT1_FUNC 0
#define PCIE_PORT2_DEV 0x0a
#define PCIE_PORT2_FUNC 0
#define PCIE_PORT3_DEV 0x0b
#define PCIE_PORT3_FUNC 0
#define PCIE_PORT4_DEV 0x0c
#define PCIE_PORT4_FUNC 0
#define PCIE_PORT5_DEV 0x0e
#define PCIE_PORT5_FUNC 0
#define PCIE_PORT6_DEV 0x0f
#define PCIE_PORT6_FUNC 0
#define PCIE_PORT7_DEV 0x10
#define PCIE_PORT7_FUNC 0
#define PCIE_PORT8_DEV 0x11
#define PCIE_PORT8_FUNC 0

/* SMBUS 2 */
#define SMBUS2_DEV 0x12
#define SMBUS2_FUNC 0

/* SATA */
#define SATA_DEV 0x13
#define SATA_FUNC 0
#define SATA2_DEV 0x14
#define SATA2_FUNC 0

/* xHCI */
#define XHCI_DEV 0x15
#define XHCI_FUNC 0

/* Virtual Root Port 0 */
#define VRP0_DEV 0x16
#define VRP0_FUNC 0

/* Virtual Root Port 1 */
#define VRP1_DEV 0x17
#define VRP1_FUNC 0

/* CSME */
#define ME_HECI_DEV 0x18
#define ME_HECI1_DEV ME_HECI_DEV
#define ME_HECI1_FUNC 0
#define ME_HECI2_DEV ME_HECI_DEV
#define ME_HECI2_FUNC 1
#define ME_IEDR_DEV ME_HECI_DEV
#define ME_IEDR_FUNC 2
#define ME_MEKT_DEV ME_HECI_DEV
#define ME_MEKT_FUNC 3
#define ME_HECI3_DEV ME_HECI_DEV
#define ME_HECI3_FUNC 4

/* HSUART */
#define HSUART_DEV 0x1a
#define HSUART1_DEV HSUART_DEV
#define HSUART1_FUNC 0
#define HSUART2_DEV HSUART_DEV
#define HSUART2_FUNC 1
#define HSUART3_DEV HSUART_DEV
#define HSUART3_FUNC 2

/* IE */
#define IE_HECI_DEV 0x1b
#define IE_HECI1_DEV IE_HECI_DEV
#define IE_HECI1_FUNC 0
#define IE_HECI2_DEV IE_HECI_DEV
#define IE_HECI2_FUNC 1
#define IE_IEDR_DEV IE_HECI_DEV
#define IE_IEDR_FUNC 2
#define IE_MEKT_DEV IE_HECI_DEV
#define IE_MEKT_FUNC 3
#define IE_HECI3_DEV IE_HECI_DEV
#define IE_HECI3_FUNC 4

/* MMC Port */
#define MMC_DEV 0x1c
#define MMC_FUNC 0

/* Platform Controller Unit */
#define PCU_DEV 0x1f
#define LPC_DEV PCU_DEV
#define LPC_FUNC 0
#define P2SB_DEV PCU_DEV
#define P2SB_FUNC 1
#define PMC_DEV PCU_DEV
#define PMC_FUNC 2
#define SMBUS_DEV PCU_DEV
#define SMBUS_FUNC 4
#define SPI_DEV PCU_DEV
#define SPI_FUNC 5
#define NPK_DEV PCU_DEV
#define NPK_FUNC 7

/* TODO - New added */
#define SA_DEV_SLOT_ROOT 0x00
#define  SA_DEVFN_ROOT	PCI_DEVFN(SA_DEV_SLOT_ROOT, 0)
#if defined(__SIMPLE_DEVICE__)
#define  SA_DEV_ROOT	PCI_DEV(0, SA_DEV_SLOT_ROOT, 0)
#endif

#define PCH_DEV_SLOT_LPC 0x1f
#define PCH_DEVFN_LPC _PCH_DEVFN(LPC, 0)
#define PCH_DEVFN_PMC _PCH_DEVFN(LPC, 2)
#define PCH_DEVFN_SPI _PCH_DEVFN(LPC, 5)
#define PCH_DEV_LPC _PCH_DEV(LPC, 0)
#define PCH_DEV_SPI _PCH_DEV(LPC, 5)

#endif /* _DENVERTON_NS_PCI_DEVS_H_ */
