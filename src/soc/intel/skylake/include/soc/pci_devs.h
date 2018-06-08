/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#ifndef _SOC_SKYLAKE_PCI_DEVS_H_
#define _SOC_SKYLAKE_PCI_DEVS_H_

#include <device/pci_def.h>
#include <rules.h>

#define _SA_DEVFN(slot)		PCI_DEVFN(SA_DEV_SLOT_ ## slot, 0)
#define _PCH_DEVFN(slot, func)	PCI_DEVFN(PCH_DEV_SLOT_ ## slot, func)

#if !defined(__SIMPLE_DEVICE__)
#include <device/device.h>
#include <device/pci_def.h>
#define _SA_DEV(slot)		dev_find_slot(0, _SA_DEVFN(slot))
#define _PCH_DEV(slot, func)	dev_find_slot(0, _PCH_DEVFN(slot, func))
#else
#include <arch/io.h>
#define _SA_DEV(slot)		PCI_DEV(0, SA_DEV_SLOT_ ## slot, 0)
#define _PCH_DEV(slot, func)	PCI_DEV(0, PCH_DEV_SLOT_ ## slot, func)
#endif

/* System Agent Devices */

#define SA_DEV_SLOT_ROOT	0x00
#define  SA_DEVFN_ROOT		_SA_DEVFN(ROOT)
#define  SA_DEV_ROOT		_SA_DEV(ROOT)

#define SA_DEV_SLOT_IGD		0x02
#define  SA_DEVFN_IGD		_SA_DEVFN(IGD)
#define  SA_DEV_IGD		_SA_DEV(IGD)

/* PCH Devices */

#define PCH_DEV_SLOT_ISH	0x13
#define  PCH_DEVFN_ISH		_PCH_DEVFN(ISH, 0)
#define  PCH_DEV_ISH		_PCH_DEV(ISH, 0)

#define PCH_DEV_SLOT_XHCI	0x14
#define  PCH_DEVFN_XHCI		_PCH_DEVFN(XHCI, 0)
#define  PCH_DEVFN_USBOTG	_PCH_DEVFN(XHCI, 1)
#define  PCH_DEVFN_THERMAL	_PCH_DEVFN(XHCI, 2)
#define  PCH_DEVFN_CIO		_PCH_DEVFN(XHCI, 3)
#define  PCH_DEV_XHCI		_PCH_DEV(XHCI, 0)
#define  PCH_DEV_USBOTG		_PCH_DEV(XHCI, 1)
#define  PCH_DEV_THERMAL	_PCH_DEV(XHCI, 2)
#define  PCH_DEV_CIO		_PCH_DEV(XHCI, 3)

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
#define  PCH_DEV_CSE		_PCH_DEV(CSE, 0)
#define  PCH_DEV_CSE_2		_PCH_DEV(CSE, 1)
#define  PCH_DEV_CSE_IDER	_PCH_DEV(CSE, 2)
#define  PCH_DEV_CSE_KT		_PCH_DEV(CSE, 3)
#define  PCH_DEV_CSE_3		_PCH_DEV(CSE, 4)

#define PCH_DEV_SLOT_SATA	0x17
#define  PCH_DEVFN_SATA		_PCH_DEVFN(SATA, 0)
#define  PCH_DEV_SATA		_PCH_DEV(SATA, 0)

#define PCH_DEV_SLOT_SIO2	0x19
#define  PCH_DEVFN_UART2	_PCH_DEVFN(SIO2, 0)
#define  PCH_DEVFN_I2C5		_PCH_DEVFN(SIO2, 1)
#define  PCH_DEVFN_I2C4		_PCH_DEVFN(SIO2, 2)
#define  PCH_DEV_UART2		_PCH_DEV(SIO2, 0)
#define  PCH_DEV_I2C5		_PCH_DEV(SIO2, 1)
#define  PCH_DEV_I2C4		_PCH_DEV(SIO2, 2)

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

#define PCH_DEV_SLOT_STORAGE	0x1e
#define  PCH_DEVFN_UART0	_PCH_DEVFN(STORAGE, 0)
#define  PCH_DEVFN_UART1	_PCH_DEVFN(STORAGE, 1)
#define  PCH_DEVFN_GSPI0	_PCH_DEVFN(STORAGE, 2)
#define  PCH_DEVFN_GSPI1	_PCH_DEVFN(STORAGE, 3)
#define  PCH_DEVFN_EMMC		_PCH_DEVFN(STORAGE, 4)
#define  PCH_DEVFN_SDIO		_PCH_DEVFN(STORAGE, 5)
#define  PCH_DEVFN_SDCARD	_PCH_DEVFN(STORAGE, 6)
#define  PCH_DEV_UART0		_PCH_DEV(STORAGE, 0)
#define  PCH_DEV_UART1		_PCH_DEV(STORAGE, 1)
#define  PCH_DEV_GSPI0		_PCH_DEV(STORAGE, 2)
#define  PCH_DEV_GSPI1		_PCH_DEV(STORAGE, 3)
#define  PCH_DEV_EMMC		_PCH_DEV(STORAGE, 4)
#define  PCH_DEV_SDIO		_PCH_DEV(STORAGE, 5)
#define  PCH_DEV_SDCARD		_PCH_DEV(STORAGE, 6)

#define PCH_DEV_SLOT_LPC	0x1f
#define  PCH_DEVFN_LPC		_PCH_DEVFN(LPC, 0)
#define  PCH_DEVFN_P2SB         _PCH_DEVFN(LPC, 1)
#define  PCH_DEVFN_PMC		_PCH_DEVFN(LPC, 2)
#define  PCH_DEVFN_HDA		_PCH_DEVFN(LPC, 3)
#define  PCH_DEVFN_SMBUS	_PCH_DEVFN(LPC, 4)
#define  PCH_DEVFN_SPI		_PCH_DEVFN(LPC, 5)
#define  PCH_DEVFN_GBE		_PCH_DEVFN(LPC, 6)
#define  PCH_DEVFN_TRACEHUB	_PCH_DEVFN(LPC, 7)
#define  PCH_DEV_LPC		_PCH_DEV(LPC, 0)
#define  PCH_DEV_P2SB		_PCH_DEV(LPC, 1)
#define  PCH_DEV_PMC		_PCH_DEV(LPC, 2)
#define  PCH_DEV_HDA		_PCH_DEV(LPC, 3)
#define  PCH_DEV_SMBUS		_PCH_DEV(LPC, 4)
#define  PCH_DEV_SPI		_PCH_DEV(LPC, 5)
#define  PCH_DEV_GBE		_PCH_DEV(LPC, 6)
#define  PCH_DEV_TRACEHUB	_PCH_DEV(LPC, 7)

#endif
