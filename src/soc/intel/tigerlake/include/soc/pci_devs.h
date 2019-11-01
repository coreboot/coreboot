/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Intel Corporation.
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

#ifndef _SOC_TIGERLAKE_PCI_DEVS_H_
#define _SOC_TIGERLAKE_PCI_DEVS_H_

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

#define SA_DEV_SLOT_DSP		0x04
#define  SA_DEVFN_DSP		PCI_DEVFN(SA_DEV_SLOT_DSP, 0)
#define  SA_DEV_DSP		PCI_DEV(0, SA_DEV_SLOT_DSP, 0)

/* PCH Devices */
#define PCH_DEV_SLOT_THERMAL	0x12
#define  PCH_DEVFN_THERMAL	_PCH_DEVFN(THERMAL, 0)
#define  PCH_DEVFN_UFS		_PCH_DEVFN(THERMAL, 5)
#define  PCH_DEVFN_GSPI2	_PCH_DEVFN(THERMAL, 6)
#define  PCH_DEV_THERMAL	_PCH_DEV(THERMAL, 0)
#define  PCH_DEV_UFS		_PCH_DEV(THERMAL, 5)
#define  PCH_DEV_GSPI2		_PCH_DEV(THERMAL, 6)

#define PCH_DEV_SLOT_ISH	0x13
#define  PCH_DEVFN_ISH		_PCH_DEVFN(ISH, 0)
#define  PCH_DEV_ISH		_PCH_DEV(ISH, 0)

#define PCH_DEV_SLOT_XHCI	0x14
#define  PCH_DEVFN_XHCI		_PCH_DEVFN(XHCI, 0)
#define  PCH_DEVFN_USBOTG	_PCH_DEVFN(XHCI, 1)
#define  PCH_DEVFN_CNViWIFI	_PCH_DEVFN(XHCI, 3)
#define  PCH_DEVFN_SDCARD	_PCH_DEVFN(XHCI, 5)
#define  PCH_DEV_XHCI		_PCH_DEV(XHCI, 0)
#define  PCH_DEV_USBOTG		_PCH_DEV(XHCI, 1)
#define  PCH_DEV_CNViWIFI	_PCH_DEV(XHCI, 3)
#define  PCH_DEV_SDCARD		_PCH_DEV(XHCI, 5)

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
#define  PCH_DEVFN_CSE_IDER	_PCH_DEVFN(CSE, 2)
#define  PCH_DEVFN_CSE_KT	_PCH_DEVFN(CSE, 3)
#define  PCH_DEVFN_CSE_3	_PCH_DEVFN(CSE, 4)
#define  PCH_DEVFN_CSE_4	_PCH_DEVFN(CSE, 5)
#define  PCH_DEV_CSE		_PCH_DEV(CSE, 0)
#define  PCH_DEV_CSE_2		_PCH_DEV(CSE, 1)
#define  PCH_DEV_CSE_IDER	_PCH_DEV(CSE, 2)
#define  PCH_DEV_CSE_KT		_PCH_DEV(CSE, 3)
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

#define PCH_DEV_SLOT_STORAGE	0x1A
#define  PCH_DEVFN_EMMC		_PCH_DEVFN(STORAGE, 0)
#define  PCH_DEV_EMMC		_PCH_DEV(STORAGE, 0)

#define PCH_DEV_SLOT_PCIE	0x1c
#define  PCH_DEVFN_PCIE1	_PCH_DEVFN(PCIE, 0)
#define  PCH_DEVFN_PCIE2	_PCH_DEVFN(PCIE, 1)
#define  PCH_DEVFN_PCIE3	_PCH_DEVFN(PCIE, 2)
#define  PCH_DEVFN_PCIE4	_PCH_DEVFN(PCIE, 3)
#define  PCH_DEVFN_PCIE5	_PCH_DEVFN(PCIE, 4)
#define  PCH_DEVFN_PCIE6	_PCH_DEVFN(PCIE, 5)
#define  PCH_DEVFN_PCIE7	_PCH_DEVFN(PCIE, 6)
#define  PCH_DEVFN_PCIE8	_PCH_DEVFN(PCIE, 7)
#define  PCH_DEV_PCIE1		_PCH_DEV(PCIE, 0)
#define  PCH_DEV_PCIE2		_PCH_DEV(PCIE, 1)
#define  PCH_DEV_PCIE3		_PCH_DEV(PCIE, 2)
#define  PCH_DEV_PCIE4		_PCH_DEV(PCIE, 3)
#define  PCH_DEV_PCIE5		_PCH_DEV(PCIE, 4)
#define  PCH_DEV_PCIE6		_PCH_DEV(PCIE, 5)
#define  PCH_DEV_PCIE7		_PCH_DEV(PCIE, 6)
#define  PCH_DEV_PCIE8		_PCH_DEV(PCIE, 7)

#define PCH_DEV_SLOT_PCIE_1	0x1d
#define  PCH_DEVFN_PCIE9	_PCH_DEVFN(PCIE_1, 0)
#define  PCH_DEVFN_PCIE10	_PCH_DEVFN(PCIE_1, 1)
#define  PCH_DEVFN_PCIE11	_PCH_DEVFN(PCIE_1, 2)
#define  PCH_DEVFN_PCIE12	_PCH_DEVFN(PCIE_1, 3)
#define  PCH_DEVFN_PCIE13	_PCH_DEVFN(PCIE_1, 4)
#define  PCH_DEVFN_PCIE14	_PCH_DEVFN(PCIE_1, 5)
#define  PCH_DEVFN_PCIE15	_PCH_DEVFN(PCIE_1, 6)
#define  PCH_DEVFN_PCIE16	_PCH_DEVFN(PCIE_1, 7)
#define  PCH_DEV_PCIE9		_PCH_DEV(PCIE_1, 0)
#define  PCH_DEV_PCIE10		_PCH_DEV(PCIE_1, 1)
#define  PCH_DEV_PCIE11		_PCH_DEV(PCIE_1, 2)
#define  PCH_DEV_PCIE12		_PCH_DEV(PCIE_1, 3)
#define  PCH_DEV_PCIE13		_PCH_DEV(PCIE_1, 4)
#define  PCH_DEV_PCIE14		_PCH_DEV(PCIE_1, 5)
#define  PCH_DEV_PCIE15		_PCH_DEV(PCIE_1, 6)
#define  PCH_DEV_PCIE16		_PCH_DEV(PCIE_1, 7)

#define PCH_DEV_SLOT_PCIE_2	0x1b
#define  PCH_DEVFN_PCIE17	_PCH_DEVFN(PCIE_2, 0)
#define  PCH_DEVFN_PCIE18	_PCH_DEVFN(PCIE_2, 1)
#define  PCH_DEVFN_PCIE19	_PCH_DEVFN(PCIE_2, 2)
#define  PCH_DEVFN_PCIE20	_PCH_DEVFN(PCIE_2, 3)
#define  PCH_DEVFN_PCIE21	_PCH_DEVFN(PCIE_2, 4)
#define  PCH_DEVFN_PCIE22	_PCH_DEVFN(PCIE_2, 5)
#define  PCH_DEVFN_PCIE23	_PCH_DEVFN(PCIE_2, 6)
#define  PCH_DEVFN_PCIE24	_PCH_DEVFN(PCIE_2, 7)
#define  PCH_DEV_PCIE17		_PCH_DEV(PCIE_2, 0)
#define  PCH_DEV_PCIE18		_PCH_DEV(PCIE_2, 1)
#define  PCH_DEV_PCIE19		_PCH_DEV(PCIE_2, 2)
#define  PCH_DEV_PCIE20		_PCH_DEV(PCIE_2, 3)
#define  PCH_DEV_PCIE21		_PCH_DEV(PCIE_2, 4)
#define  PCH_DEV_PCIE22		_PCH_DEV(PCIE_2, 5)
#define  PCH_DEV_PCIE23		_PCH_DEV(PCIE_2, 6)
#define  PCH_DEV_PCIE24		_PCH_DEV(PCIE_2, 7)

#define PCH_DEV_SLOT_SIO3	0x1e
#define  PCH_DEVFN_UART0	_PCH_DEVFN(SIO3, 0)
#define  PCH_DEVFN_UART1	_PCH_DEVFN(SIO3, 1)
#define  PCH_DEVFN_GSPI0	_PCH_DEVFN(SIO3, 2)
#define  PCH_DEVFN_GSPI1	_PCH_DEVFN(SIO3, 3)
#define  PCH_DEV_UART0		_PCH_DEV(SIO3, 0)
#define  PCH_DEV_UART1		_PCH_DEV(SIO3, 1)
#define  PCH_DEV_GSPI0		_PCH_DEV(SIO3, 2)
#define  PCH_DEV_GSPI1		_PCH_DEV(SIO3, 3)

#define PCH_DEV_SLOT_ESPI	0x1f
#define PCH_DEV_SLOT_LPC	PCH_DEV_SLOT_ESPI
#define  PCH_DEVFN_ESPI		_PCH_DEVFN(ESPI, 0)
#define  PCH_DEVFN_P2SB		_PCH_DEVFN(ESPI, 1)
#define  PCH_DEVFN_PMC		_PCH_DEVFN(ESPI, 2)
#define  PCH_DEVFN_HDA		_PCH_DEVFN(ESPI, 3)
#define  PCH_DEVFN_SMBUS	_PCH_DEVFN(ESPI, 4)
#define  PCH_DEVFN_SPI		_PCH_DEVFN(ESPI, 5)
#define  PCH_DEVFN_GBE		_PCH_DEVFN(ESPI, 6)
#define  PCH_DEVFN_TRACEHUB	_PCH_DEVFN(ESPI, 7)
#define  PCH_DEV_ESPI	_PCH_DEV(ESPI, 0)
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
#define  PCH_DEV_GBE		_PCH_DEV(ESPI, 6)
#define  PCH_DEV_TRACEHUB	_PCH_DEV(ESPI, 7)

#endif
