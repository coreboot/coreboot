/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_PCH_PCI_DEVS_H_
#define _SOC_PCH_PCI_DEVS_H_

#define _PCH_DEVFN(slot, func)  PCI_DEVFN(PCH_DEV_SLOT_ ## slot, func)

#if !defined(__SIMPLE_DEVICE__)
#define _PCH_DEV(slot, func)    pcidev_path_on_root_debug(_PCH_DEVFN(slot, func), __func__)
#else
#define _PCH_DEV(slot, func)    PCI_DEV(0, PCH_DEV_SLOT_ ## slot, func)
#endif

/* PCH Device info */

#define XHCI_BUS_NUMBER		0x0
#define PCH_DEV_SLOT_XHCI	0x14
#define XHCI_FUNC_NUM		0x0
#define PCH_DEVFN_XHCI		_PCH_DEVFN(XHCI, 0)
#define PCH_DEV_XHCI		_PCH_DEV(XHCI, 0)
#define  PCH_DEVFN_THERMAL	_PCH_DEVFN(XHCI, 2)

#define HPET_BUS_NUM		0x0
#define HPET_DEV_NUM		PCH_DEV_SLOT_LPC
#define HPET0_FUNC_NUM		0x00

#define PCH_DEV_SLOT_CSE	0x16
#define  PCH_DEVFN_CSE		_PCH_DEVFN(CSE, 0)
#define  PCH_DEVFN_CSE_2	_PCH_DEVFN(CSE, 1)
#define  PCH_DEVFN_CSE_3	_PCH_DEVFN(CSE, 4)
#define  PCH_DEV_CSE		_PCH_DEV(CSE, 0)
#define  PCH_DEV_CSE_2		_PCH_DEV(CSE, 1)
#define  PCH_DEV_CSE_3		_PCH_DEV(CSE, 4)

#define PCH_DEV_SLOT_LPC	0x1f
#define  PCH_DEVFN_LPC		_PCH_DEVFN(LPC, 0)
#define  PCH_DEVFN_P2SB		_PCH_DEVFN(LPC, 1)
#define  PCH_DEVFN_PMC		_PCH_DEVFN(LPC, 2)
#define  PCH_DEVFN_SMBUS	_PCH_DEVFN(LPC, 4)
#define  PCH_DEVFN_SPI		_PCH_DEVFN(LPC, 5)
#define  PCH_DEV_LPC		_PCH_DEV(LPC, 0)
#define  PCH_DEV_P2SB		_PCH_DEV(LPC, 1)
#define  PCH_DEV_PMC		_PCH_DEV(LPC, 2)
#define  PCH_DEV_SMBUS		_PCH_DEV(LPC, 4)
#define  PCH_DEV_SPI		_PCH_DEV(LPC, 5)

#define PCH_IOAPIC_BUS_NUMBER	0xF0
#define PCH_IOAPIC_DEV_NUM	0x1F
#define PCH_IOAPIC_FUNC_NUM	0x00

// ========== IOAPIC Definitions for DMAR/ACPI ========
#define PCH_IOAPIC_ID		0x08

#endif
