/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DENVERTON_NS_PCI_DEVS_H_
#define _DENVERTON_NS_PCI_DEVS_H_

#include <device/pci_def.h>

/* All these devices live on bus 0 with the associated device and function */

#define _PCH_DEVFN(slot, func) PCI_DEVFN(PCH_DEV_SLOT_##slot, func)

#if !defined(__SIMPLE_DEVICE__)
#include <device/device.h>
#define _PCH_DEV(slot, func) pcidev_path_on_root(_PCH_DEVFN(slot, func))
#else
#define _PCH_DEV(slot, func) PCI_DEV(0, PCH_DEV_SLOT_##slot, func)
#endif

/* SoC transaction router */
#define SA_DEV_SLOT_ROOT	0x0
#define  SA_DEVFN_ROOT		PCI_DEVFN(SA_DEV_SLOT_ROOT, 0)
#if defined(__SIMPLE_DEVICE__)
#define  SA_DEV_ROOT		PCI_DEV(0, SA_DEV_SLOT_ROOT, 0)
#else
#include <device/device.h>
#define  SA_DEV_ROOT		pcidev_path_on_root(PCI_DEVFN(SA_DEV_SLOT_ROOT, 0))
#endif
#define SA_DEV 0x0
#define SA_FUNC 0
#define SOC_DEV SA_DEV
#define SOC_FUNC SA_FUNC

/* RAS */
#define RAS_DEV 0x4
#define RAS_FUNC 0

/* Root Complex Event Collector */
#define PCH_DEV_SLOT_RCEC	0x5
#define  PCH_DEVFN_RCEC		_PCH_DEVFN(RCEC, 0)
#define  PCH_DEV_RCEC		_PCH_DEV(RCEC, 0)
#define RCEC_DEV 0x5
#define RCEC_FUNC 0

/* Virtual Root Port 2 */
#define PCH_DEV_SLOT_QAT	0x6
#define  PCH_DEVFN_QAT		_PCH_DEVFN(QAT, 0)
#define  PCH_DEV_QAT		_PCH_DEV(QAT, 0)
#define VRP2_DEV 0x6
#define VRP2_FUNC 0

/* PCIe Root Ports */
#define PCH_DEV_SLOT_PCIE1	0x9
#define  PCH_DEVFN_PCIE1	_PCH_DEVFN(PCIE1, 0)
#define  PCH_DEV_PCIE1		_PCH_DEV(PCIE1, 0)
#define PCH_DEV_SLOT_PCIE2	0xa
#define  PCH_DEVFN_PCIE2	_PCH_DEVFN(PCIE2, 0)
#define  PCH_DEV_PCIE2		_PCH_DEV(PCIE2, 0)
#define PCH_DEV_SLOT_PCIE3	0xb
#define  PCH_DEVFN_PCIE3	_PCH_DEVFN(PCIE3, 0)
#define  PCH_DEV_PCIE3		_PCH_DEV(PCIE3, 0)
#define PCH_DEV_SLOT_PCIE4	0xc
#define  PCH_DEVFN_PCIE4	_PCH_DEVFN(PCIE4, 0)
#define  PCH_DEV_PCIE4		_PCH_DEV(PCIE4, 0)
#define PCH_DEV_SLOT_PCIE5	0xe
#define  PCH_DEVFN_PCIE5	_PCH_DEVFN(PCIE5, 0)
#define  PCH_DEV_PCIE5		_PCH_DEV(PCIE5, 0)
#define PCH_DEV_SLOT_PCIE6	0xf
#define  PCH_DEVFN_PCIE6	_PCH_DEVFN(PCIE6, 0)
#define  PCH_DEV_PCIE6		_PCH_DEV(PCIE6, 0)
#define PCH_DEV_SLOT_PCIE7	0x10
#define  PCH_DEVFN_PCIE7	_PCH_DEVFN(PCIE7, 0)
#define  PCH_DEV_PCIE7		_PCH_DEV(PCIE7, 0)
#define PCH_DEV_SLOT_PCIE8	0x11
#define  PCH_DEVFN_PCIE8	_PCH_DEVFN(PCIE8, 0)
#define  PCH_DEV_PCIE8		_PCH_DEV(PCIE8, 0)

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
#define PCH_DEV_SLOT_SMBUS_HOST	0x12
#define  PCH_DEVFN_SMBUS_HOST	_PCH_DEVFN(SMBUS_HOST, 0)
#define  PCH_DEV_SMBUS_HOST	_PCH_DEV(SMBUS_HOST, 0)
#define SMBUS2_DEV 0x12
#define SMBUS2_FUNC 0

/* SATA */
#define PCH_DEV_SLOT_SATA_0	0x13
#define  PCH_DEVFN_SATA_0	_PCH_DEVFN(SATA_0, 0)
#define  PCH_DEV_SATA_0		_PCH_DEV(SATA_0, 0)
#define PCH_DEV_SLOT_SATA_1	0x14
#define  PCH_DEVFN_SATA_1	_PCH_DEVFN(SATA_1, 0)
#define  PCH_DEV_SATA_1		_PCH_DEV(SATA_1, 0)
#define SATA_DEV 0x13
#define SATA_FUNC 0
#define SATA2_DEV 0x14
#define SATA2_FUNC 0

/* xHCI */
#define PCH_DEV_SLOT_XHCI	0x15
#define  PCH_DEVFN_XHCI		_PCH_DEVFN(XHCI, 0)
#define  PCH_DEV_XHCI		_PCH_DEV(XHCI, 0)
#define XHCI_DEV 0x15
#define XHCI_FUNC 0

/* Virtual Root Port 0 */
#define PCH_DEV_SLOT_LAN0	0x16
#define  PCH_DEVFN_LAN0		_PCH_DEVFN(LAN0, 0)
#define  PCH_DEV_LAN0		_PCH_DEV(LAN0, 0)
#define VRP0_DEV 0x16
#define VRP0_FUNC 0

/* Virtual Root Port 1 */
#define PCH_DEV_SLOT_LAN1	0x17
#define  PCH_DEVFN_LAN1		_PCH_DEVFN(LAN1, 0)
#define  PCH_DEV_LAN1		_PCH_DEVFN(LAN1, 0)
#define VRP1_DEV 0x17
#define VRP1_FUNC 0

/* CSME */
#define PCH_DEV_SLOT_ME		0x18
#define  PCH_DEVFN_ME_HECI1	 _PCH_DEVFN(ME, 0)
#define  PCH_DEVFN_ME_HECI2	 _PCH_DEVFN(ME, 1)
#define  PCH_DEVFN_ME_HECI3	 _PCH_DEVFN(ME, 4)
#define  PCH_DEVFN_ME_KT	_PCH_DEVFN(ME, 3)
#define  PCH_DEV_ME_HECI1	_PCH_DEV(ME, 0)
#define  PCH_DEV_ME_HECI2	_PCH_DEV(ME, 1)
#define  PCH_DEV_ME_HECI3	_PCH_DEV(ME, 4)
#define  PCH_DEV_ME_KT		_PCH_DEV(ME, 3)

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
#define PCH_DEV_SLOT_UART	0x1a
#define  PCH_DEVFN_UART0	_PCH_DEVFN(UART, 0)
#define  PCH_DEVFN_UART1	_PCH_DEVFN(UART, 1)
#define  PCH_DEVFN_UART2	_PCH_DEVFN(UART, 2)
#define  PCH_DEV_UART0		_PCH_DEV(UART, 0)
#define  PCH_DEV_UART1		_PCH_DEV(UART, 1)
#define  PCH_DEV_UART2		_PCH_DEV(UART, 2)
#define HSUART_DEV 0x1a
#define HSUART1_DEV HSUART_DEV
#define HSUART1_FUNC 0
#define HSUART2_DEV HSUART_DEV
#define HSUART2_FUNC 1
#define HSUART3_DEV HSUART_DEV
#define HSUART3_FUNC 2

/* IE */
#define PCH_DEV_SLOT_IE		0x1b
#define  PCH_DEVFN_IE_HECI1	_PCH_DEVFN(IE, 0)
#define  PCH_DEVFN_IE_HECI2	_PCH_DEVFN(IE, 1)
#define  PCH_DEVFN_IE_HECI3	_PCH_DEVFN(IE, 4)
#define  PCH_DEVFN_IE_KT	_PCH_DEVFN(IE, 3)
#define  PCH_DEV_IE_HECI1	_PCH_DEV(IE, 0)
#define  PCH_DEV_IE_HECI2	_PCH_DEV(IE, 1)
#define  PCH_DEV_IE_HECI3	_PCH_DEV(IE, 4)
#define  PCH_DEV_IE_KT		_PCH_DEV(IE, 3)
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
#define PCH_DEV_SLOT_EMMC	0x1c
#define  PCH_DEVFN_EMMC		_PCH_DEVFN(EMMC, 0)
#define  PCH_DEV_EMMC		_PCH_DEV(EMMC, 0)
#define MMC_DEV 0x1c
#define MMC_FUNC 0

/* Platform Controller Unit */
#define PCH_DEV_SLOT_LPC	0x1f
#define  PCH_DEVFN_LPC		_PCH_DEVFN(LPC, 0)
#define  PCH_DEVFN_P2SB		_PCH_DEVFN(LPC, 1)
#define  PCH_DEVFN_PMC		_PCH_DEVFN(LPC, 2)
#define  PCH_DEVFN_SMBUS	_PCH_DEVFN(LPC, 4)
#define  PCH_DEVFN_SPI		_PCH_DEVFN(LPC, 5)
#define  PCH_DEVFN_TRACE	_PCH_DEVFN(LPC, 7)
#define  PCH_DEV_LPC		_PCH_DEV(LPC, 0)
#define  PCH_DEV_P2SB		_PCH_DEV(LPC, 1)
#define  PCH_DEV_PMC		_PCH_DEV(LPC, 2)
#define  PCH_DEV_SMBUS		_PCH_DEV(LPC, 4)
#define  PCH_DEV_SPI		_PCH_DEV(LPC, 5)
#define  PCH_DEV_TRACE		_PCH_DEV(LPC, 7)
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

/* VT-d support value to match FSP settings */
/* "PCH IOAPIC Config" */
#define PCH_IOAPIC_PCI_BUS	0xf0
#define PCH_IOAPIC_PCI_SLOT	0x1f
/* "PCH HPET Config" */
#define PCH_HPET_PCI_BUS	0
#define PCH_HPET_PCI_SLOT	0

#endif /* _DENVERTON_NS_PCI_DEVS_H_ */
