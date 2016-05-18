/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_APOLLOLAKE_PCI_DEVS_H_
#define _SOC_APOLLOLAKE_PCI_DEVS_H_

#include <rules.h>

#define _LPSS_PCI_DEVFN(slot, func)		PCI_DEVFN(LPSS_DEV_SLOT_##slot, func)

#if !defined(__SIMPLE_DEVICE__)
#include <device/device.h>
#include <device/pci_def.h>
#define _LPSS_PCI_DEV(slot, func)		dev_find_slot(0, _LPSS_PCI_DEVFN(slot, func))
#else
#include <arch/io.h>
#define _LPSS_PCI_DEV(slot, func)		PCI_DEV(0, LPSS_DEV_SLOT_##slot, func)
#endif

/* LPSS UART */
#define LPSS_DEV_SLOT_UART	0x18
#define  LPSS_DEVFN_UART0	_LPSS_PCI_DEVFN(UART, 0)
#define  LPSS_DEVFN_UART1	_LPSS_PCI_DEVFN(UART, 1)
#define  LPSS_DEVFN_UART2	_LPSS_PCI_DEVFN(UART, 2)
#define  LPSS_DEVFN_UART3	_LPSS_PCI_DEVFN(UART, 3)
#define  LPSS_DEV_UART0		_LPSS_PCI_DEV(UART, 0)
#define  LPSS_DEV_UART1		_LPSS_PCI_DEV(UART, 1)
#define  LPSS_DEV_UART2		_LPSS_PCI_DEV(UART, 2)
#define  LPSS_DEV_UART3		_LPSS_PCI_DEV(UART, 3)

#define NB_BUS			0
#define NB_DEVFN		PCI_DEVFN(0, 0)
#define NB_DEV_ROOT		PCI_DEV(NB_BUS, 0x0, 0)
#define P2SB_DEV		PCI_DEV(0, 0xd, 0)
#define PMC_DEV			PCI_DEV(0, 0xd, 1)
#define SPI_DEV			PCI_DEV(0, 0xd, 2)
#define LPC_DEV			PCI_DEV(0, 0x1f, 0)

#endif
