/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_ELKHARTLAKE_PCI_DEVS_H_
#define _SOC_ELKHARTLAKE_PCI_DEVS_H_

#include <device/pci_def.h>

#define _PCH_DEVFN(slot, func)	PCI_DEVFN(PCH_DEV_SLOT_ ## slot, func)

#if !defined(__SIMPLE_DEVICE__)
#include <device/device.h>
#define _PCH_DEV(slot, func)	pcidev_path_on_root_debug(_PCH_DEVFN(slot, func), __func__)
#else
#define _PCH_DEV(slot, func)	PCI_DEV(0, PCH_DEV_SLOT_ ## slot, func)
#endif

/* System Agent Devices */

#define SA_DEV_SLOT_ROOT	0x00
#define  SA_DEVFN_ROOT		PCI_DEVFN(SA_DEV_SLOT_ROOT, 0)
#if defined(__SIMPLE_DEVICE__)
#define  SA_DEV_ROOT		PCI_DEV(0, SA_DEV_SLOT_ROOT, 0)
#endif

#define SA_DEV_SLOT_IGD		0x02
#define  SA_DEVFN_IGD		PCI_DEVFN(SA_DEV_SLOT_IGD, 0)
#define  SA_DEV_IGD		PCI_DEV(0, SA_DEV_SLOT_IGD, 0)

#define SA_DEV_SLOT_DPTF	0x04
#define  SA_DEVFN_DPTF		PCI_DEVFN(SA_DEV_SLOT_DPTF, 0)
#define  SA_DEV_DPTF		PCI_DEV(0, SA_DEV_SLOT_DPTF, 0)

#define SA_DEV_SLOT_GNA		0x08
#define  SA_DEVFN_GNA		PCI_DEVFN(SA_DEV_SLOT_GNA, 0)
#define  SA_DEV_GNA		PCI_DEV(0, SA_DEV_SLOT_GNA, 0)

#define SA_DEV_SLOT_TRACEHUB	0x09
#define  SA_DEVFN_TRACEHUB	PCI_DEVFN(SA_DEV_SLOT_TRACEHUB, 0)
#define  SA_DEV_TRACEHUB	PCI_DEV(0, SA_DEV_SLOT_TRACEHUB, 0)

/* PCH Devices */
#define PCH_DEV_SLOT_IEH	0x10
#define  PCH_DEVFN_I2C6		_PCH_DEVFN(IEH, 0)
#define  PCH_DEVFN_I2C7		_PCH_DEVFN(IEH, 1)
#define  PCH_DEVFN_IEH		_PCH_DEVFN(IEH, 5)
#define  PCH_DEV_I2C6		_PCH_DEV(IEH, 0)
#define  PCH_DEV_I2C7		_PCH_DEV(IEH, 1)
#define  PCH_DEV_IEH		_PCH_DEV(IEH, 5)

#define PCH_DEV_SLOT_SIO0	0x12
#define  PCH_DEVFN_GSPI2	_PCH_DEVFN(SIO0, 0)
#define  PCH_DEVFN_CSE_UMA	_PCH_DEVFN(SIO0, 3)
#define  PCH_DEVFN_CSE_DMA	_PCH_DEVFN(SIO0, 4)
#define  PCH_DEVFN_UFS0		_PCH_DEVFN(SIO0, 5)
#define  PCH_DEVFN_UFS1		_PCH_DEVFN(SIO0, 7)
#define  PCH_DEV_GSPI2		_PCH_DEV(SIO0, 0)
#define  PCH_DEV_CSE_UMA	_PCH_DEV(SIO0, 3)
#define  PCH_DEV_CSE_DMA	_PCH_DEV(SIO0, 4)
#define  PCH_DEV_UFS0		_PCH_DEV(SIO0, 5)
#define  PCH_DEV_UFS1		_PCH_DEV(SIO0, 7)

#define PCH_DEV_SLOT_XHCI	0x14
#define  PCH_DEVFN_XHCI		_PCH_DEVFN(XHCI, 0)
#define  PCH_DEVFN_USBOTG	_PCH_DEVFN(XHCI, 1)
#define  PCH_DEV_XHCI		_PCH_DEV(XHCI, 0)
#define  PCH_DEV_USBOTG		_PCH_DEV(XHCI, 1)

#define PCH_DEV_SLOT_SIO1	0x15
#define  PCH_DEVFN_I2C0		_PCH_DEVFN(SIO1, 0)
#define  PCH_DEVFN_I2C1		_PCH_DEVFN(SIO1, 1)
#define  PCH_DEVFN_I2C2		_PCH_DEVFN(SIO1, 2)
#define  PCH_DEVFN_I2C3		_PCH_DEVFN(SIO1, 3)
#define  PCH_DEV_I2C0		_PCH_DEV(SIO1, 0)
#define  PCH_DEV_I2C1		_PCH_DEV(SIO1, 1)
#define  PCH_DEV_I2C2		_PCH_DEV(SIO1, 2)
#define  PCH_DEV_I2C3		_PCH_DEV(SIO1, 3)

#define PCH_DEV_SLOT_CSE	0x16
#define  PCH_DEVFN_CSE		_PCH_DEVFN(CSE, 0)
#define  PCH_DEVFN_CSE_2	_PCH_DEVFN(CSE, 1)
#define  PCH_DEVFN_CSE_3	_PCH_DEVFN(CSE, 4)
#define  PCH_DEVFN_CSE_4	_PCH_DEVFN(CSE, 5)
#define  PCH_DEV_CSE		_PCH_DEV(CSE, 0)
#define  PCH_DEV_CSE_2		_PCH_DEV(CSE, 1)
#define  PCH_DEV_CSE_3		_PCH_DEV(CSE, 4)
#define  PCH_DEV_CSE_4		_PCH_DEV(CSE, 5)

#define PCH_DEV_SLOT_SATA	0x17
#define  PCH_DEVFN_SATA		_PCH_DEVFN(SATA, 0)
#define  PCH_DEV_SATA		_PCH_DEV(SATA, 0)

#define PCH_DEV_SLOT_SIO2	0x19
#define  PCH_DEVFN_I2C4		_PCH_DEVFN(SIO2, 0)
#define  PCH_DEVFN_I2C5		_PCH_DEVFN(SIO2, 1)
#define  PCH_DEVFN_UART2	_PCH_DEVFN(SIO2, 2)
#define  PCH_DEV_I2C4		_PCH_DEV(SIO2, 0)
#define  PCH_DEV_I2C5		_PCH_DEV(SIO2, 1)
#define  PCH_DEV_UART2		_PCH_DEV(SIO2, 2)

#define PCH_DEV_SLOT_STORAGE	0x1a
#define  PCH_DEVFN_EMMC		_PCH_DEVFN(STORAGE, 0)
#define  PCH_DEVFN_SDCARD	_PCH_DEVFN(STORAGE, 1)
#define  PCH_DEV_EMMC		_PCH_DEV(STORAGE, 0)
#define  PCH_DEV_SDCARD		_PCH_DEV(STORAGE, 1)

#define PCH_DEV_SLOT_PCIE	0x1c
#define  PCH_DEVFN_PCIE1	_PCH_DEVFN(PCIE, 0)
#define  PCH_DEVFN_PCIE2	_PCH_DEVFN(PCIE, 1)
#define  PCH_DEVFN_PCIE3	_PCH_DEVFN(PCIE, 2)
#define  PCH_DEVFN_PCIE4	_PCH_DEVFN(PCIE, 3)
#define  PCH_DEVFN_PCIE5	_PCH_DEVFN(PCIE, 4)
#define  PCH_DEVFN_PCIE6	_PCH_DEVFN(PCIE, 5)
#define  PCH_DEVFN_PCIE7	_PCH_DEVFN(PCIE, 6)
#define  PCH_DEV_PCIE1		_PCH_DEV(PCIE, 0)
#define  PCH_DEV_PCIE2		_PCH_DEV(PCIE, 1)
#define  PCH_DEV_PCIE3		_PCH_DEV(PCIE, 2)
#define  PCH_DEV_PCIE4		_PCH_DEV(PCIE, 3)
#define  PCH_DEV_PCIE5		_PCH_DEV(PCIE, 4)
#define  PCH_DEV_PCIE6		_PCH_DEV(PCIE, 5)
#define  PCH_DEV_PCIE7		_PCH_DEV(PCIE, 6)

#define PCH_DEV_SLOT_SIO3	0x1e
#define  PCH_DEVFN_UART0	_PCH_DEVFN(SIO3, 0)
#define  PCH_DEVFN_UART1	_PCH_DEVFN(SIO3, 1)
#define  PCH_DEVFN_GSPI0	_PCH_DEVFN(SIO3, 2)
#define  PCH_DEVFN_GSPI1	_PCH_DEVFN(SIO3, 3)
#define  PCH_DEVFN_GBE		_PCH_DEVFN(SIO3, 4)
#define  PCH_DEVFN_HPET		_PCH_DEVFN(SIO3, 6)
#define  PCH_DEVFN_IOAPIC	_PCH_DEVFN(SIO3, 7)
#define  PCH_DEV_UART0		_PCH_DEV(SIO3, 0)
#define  PCH_DEV_UART1		_PCH_DEV(SIO3, 1)
#define  PCH_DEV_GSPI0		_PCH_DEV(SIO3, 2)
#define  PCH_DEV_GSPI1		_PCH_DEV(SIO3, 3)
#define  PCH_DEV_GBE		_PCH_DEV(SIO3, 4)
#define  PCH_DEV_HPET		_PCH_DEV(SIO3, 6)
#define  PCH_DEV_IOAPIC		_PCH_DEV(SIO3, 7)

#define PCH_DEV_SLOT_ESPI	0x1f
#define PCH_DEV_SLOT_LPC	PCH_DEV_SLOT_ESPI
#define  PCH_DEVFN_ESPI		_PCH_DEVFN(ESPI, 0)
#define  PCH_DEVFN_P2SB		_PCH_DEVFN(ESPI, 1)
#define  PCH_DEVFN_PMC		_PCH_DEVFN(ESPI, 2)
#define  PCH_DEVFN_HDA		_PCH_DEVFN(ESPI, 3)
#define  PCH_DEVFN_SMBUS	_PCH_DEVFN(ESPI, 4)
#define  PCH_DEVFN_SPI		_PCH_DEVFN(ESPI, 5)
#define  PCH_DEVFN_TRACEHUB	_PCH_DEVFN(ESPI, 7)
#define  PCH_DEV_ESPI		_PCH_DEV(ESPI, 0)
#define  PCH_DEV_LPC		PCH_DEV_ESPI
#define  PCH_DEV_P2SB		_PCH_DEV(ESPI, 1)

#if !ENV_RAMSTAGE
/*
 * PCH_DEV_PMC is intentionally not defined in RAMSTAGE since PMC device gets
 * hidden from PCI bus after call to FSP-S. This leads to resource allocator
 * dropping it from the root bus as unused device. All references to PCH_DEV_PMC
 * would then return NULL and can go unnoticed if not handled properly. Since,
 * this device does not have any special chip config associated with it, it is
 * okay to not provide the definition for it in ramstage.
 */
#define  PCH_DEV_PMC		_PCH_DEV(ESPI, 2)
#endif

#define  PCH_DEV_HDA		_PCH_DEV(ESPI, 3)
#define  PCH_DEV_SMBUS		_PCH_DEV(ESPI, 4)
#define  PCH_DEV_SPI		_PCH_DEV(ESPI, 5)
#define  PCH_DEV_TRACEHUB	_PCH_DEV(ESPI, 7)

#endif
