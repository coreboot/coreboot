/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BROADWELL_PCI_DEVS_H_
#define _BROADWELL_PCI_DEVS_H_

#define _PCH_DEVFN(slot, func)	PCI_DEVFN(PCH_DEV_SLOT_ ## slot, func)

#if defined(__SIMPLE_DEVICE__)
#define _PCH_DEV(slot, func)	PCI_DEV(0, PCH_DEV_SLOT_ ## slot, func)
#else
#include <device/device.h>
#include <device/pci_def.h>
#define _PCH_DEV(slot, func)	pcidev_path_on_root_debug(_PCH_DEVFN(slot, func), __func__)
#endif

/* System Agent Devices */

#define SA_DEV_SLOT_ROOT	0x00
#define  SA_DEVFN_ROOT		PCI_DEVFN(SA_DEV_SLOT_ROOT, 0)
#define  HOST_BRIDGE		PCI_DEV(0, SA_DEV_SLOT_ROOT, 0)

#define SA_DEV_SLOT_IGD		0x02
#define  SA_DEVFN_IGD		PCI_DEVFN(SA_DEV_SLOT_IGD, 0)
#define  SA_DEV_IGD		PCI_DEV(0, SA_DEV_SLOT_IGD, 0)

#define SA_DEV_SLOT_MINIHD	0x03
#define  SA_DEVFN_MINIHD	PCI_DEVFN(SA_DEV_SLOT_MINIHD, 0)
#define  SA_DEV_MINIHD		PCI_DEV(0, SA_DEV_SLOT_MINIHD, 0)

/* PCH Devices */

#define PCH_DEV_SLOT_ADSP	0x13
#define  PCH_DEVFN_ADSP		_PCH_DEVFN(ADSP, 0)

#define PCH_DEV_SLOT_XHCI	0x14
#define  PCH_DEVFN_XHCI		_PCH_DEVFN(XHCI, 0)
#define  PCH_DEV_XHCI		_PCH_DEV(XHCI, 0)

#define PCH_DEV_SLOT_SIO	0x15
#define  PCH_DEV_UART0		_PCH_DEV(SIO, 5)
#define  PCH_DEV_UART1		_PCH_DEV(SIO, 6)
#define  PCH_DEVFN_SDMA		_PCH_DEVFN(SIO, 0)
#define  PCH_DEVFN_I2C0		_PCH_DEVFN(SIO, 1)
#define  PCH_DEVFN_I2C1		_PCH_DEVFN(SIO, 2)
#define  PCH_DEVFN_SPI0		_PCH_DEVFN(SIO, 3)
#define  PCH_DEVFN_SPI1		_PCH_DEVFN(SIO, 4)
#define  PCH_DEVFN_UART0	_PCH_DEVFN(SIO, 5)
#define  PCH_DEVFN_UART1	_PCH_DEVFN(SIO, 6)

#define PCH_DEV_SLOT_ME		0x16
#define  PCH_DEVFN_ME		_PCH_DEVFN(ME, 0)
#define  PCH_DEVFN_ME_2		_PCH_DEVFN(ME, 1)
#define  PCH_DEVFN_ME_IDER	_PCH_DEVFN(ME, 2)
#define  PCH_DEVFN_ME_KT	_PCH_DEVFN(ME, 3)
#define  PCH_DEV_ME		_PCH_DEV(ME, 0)

#define PCH_DEV_SLOT_SDIO	0x17
#define  PCH_DEVFN_SDIO		_PCH_DEVFN(SDIO, 0)

#define PCH_DEV_SLOT_GBE	0x19
#define  PCH_DEVFN_GBE		_PCH_DEVFN(GBE, 0)

#define PCH_DEV_SLOT_HDA	0x1b
#define  PCH_DEVFN_HDA		_PCH_DEVFN(HDA, 0)

#define PCH_DEV_SLOT_PCIE	0x1c

#define PCH_DEV_SLOT_EHCI	0x1d
#define  PCH_DEVFN_EHCI		_PCH_DEVFN(EHCI, 0)

#define PCH_DEV_SLOT_LPC	0x1f
#define  PCH_DEVFN_LPC		_PCH_DEVFN(LPC, 0)
#define  PCH_DEVFN_IDE		_PCH_DEVFN(LPC, 1)
#define  PCH_DEVFN_SATA		_PCH_DEVFN(LPC, 2)
#define  PCH_DEVFN_SMBUS	_PCH_DEVFN(LPC, 3)
#define  PCH_DEVFN_SATA2	_PCH_DEVFN(LPC, 5)
#define  PCH_DEVFN_THERMAL	_PCH_DEVFN(LPC, 6)
#define  PCH_DEV_LPC		_PCH_DEV(LPC, 0)

#define PCH_IOAPIC_PCI_BUS	250
#define PCH_IOAPIC_PCI_SLOT	31
#define PCH_HPET_PCI_BUS	250
#define PCH_HPET_PCI_SLOT	15

#endif
