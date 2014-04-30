/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _BROADWELL_PCI_DEVS_H_
#define _BROADWELL_PCI_DEVS_H_

#if defined(__PRE_RAM__) || defined(__SMM__) || defined(__ROMCC__)
#include <arch/io.h>
#define _SA_DEV(slot)		PCI_DEV(0, SA_DEV_SLOT_ ## slot, 0)
#define _PCH_DEV(slot,func)	PCI_DEV(0, PCH_DEV_SLOT_ ## slot, func)
#else
#include <device/device.h>
#include <device/pci_def.h>
#define _SA_DEV(slot)		dev_find_slot(0, \
					PCI_DEVFN(SA_DEV_SLOT_ ## slot, 0))
#define _PCH_DEV(slot,func)	dev_find_slot(0, \
					PCI_DEVFN(PCH_DEV_SLOT_ ## slot, func))
#endif

/* System Agent Devices */

#define SA_DEV_SLOT_ROOT	0x00
#define SA_DEV_SLOT_IGD		0x02
#define SA_DEV_SLOT_MINIHD	0x03

#define SA_DEV_ROOT		_SA_DEV(ROOT)
#define SA_DEV_IGD		_SA_DEV(IGD)
#define SA_DEV_MINIHD		_SA_DEV(MINIHD)

/* PCH Devices */

#define PCH_DEV_SLOT_XHCI	0x14
#define PCH_DEV_SLOT_SIO	0x15
#define PCH_DEV_SLOT_ME		0x16
#define PCH_DEV_SLOT_HDA	0x1b
#define PCH_DEV_SLOT_PCIE	0x1c
#define PCH_DEV_SLOT_EHCI	0x1d
#define PCH_DEV_SLOT_LPC	0x1f

#define PCH_DEV_XHCI		_PCH_DEV(XHCI, 0)
#define PCH_DEV_UART0		_PCH_DEV(UART0, 0)
#define PCH_DEV_UART1		_PCH_DEV(UART1, 0)
#define PCH_DEV_ME		_PCH_DEV(ME, 0)
#define PCH_DEV_HDA		_PCH_DEV(HDA, 0)
#define PCH_DEV_EHCI		_PCH_DEV(EHCI, 0)
#define PCH_DEV_LPC		_PCH_DEV(LPC, 0)
#define PCH_DEV_IDE		_PCH_DEV(LPC, 1)
#define PCH_DEV_SATA		_PCH_DEV(LPC, 2)
#define PCH_DEV_SMBUS		_PCH_DEV(LPC, 3)
#define PCH_DEV_SATA2		_PCH_DEV(LPC, 5)
#define PCH_DEV_THERMAL		_PCH_DEV(LPC, 6)

#endif
