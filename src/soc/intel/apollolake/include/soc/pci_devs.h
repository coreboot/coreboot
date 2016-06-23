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

#define ISH_DEVFN		PCI_DEVFN(0x11, 0)
#define SATA_DEVFN		PCI_DEVFN(0x12, 0)
#define PCIEA0_DEVFN		PCI_DEVFN(0x13, 0)
#define PCIEA1_DEVFN		PCI_DEVFN(0x13, 1)
#define PCIEA2_DEVFN		PCI_DEVFN(0x13, 2)
#define PCIEA3_DEVFN		PCI_DEVFN(0x13, 3)
#define PCIEB0_DEVFN		PCI_DEVFN(0x14, 0)
#define PCIEB1_DEVFN		PCI_DEVFN(0x14, 1)
#define XHCI_DEVFN		PCI_DEVFN(0x15, 0)
#define XDCI_DEVFN		PCI_DEVFN(0x15, 1)
#define I2C0_DEVFN		PCI_DEVFN(0x16, 0)
#define I2C1_DEVFN		PCI_DEVFN(0x16, 1)
#define I2C2_DEVFN		PCI_DEVFN(0x16, 2)
#define I2C3_DEVFN		PCI_DEVFN(0x16, 3)
#define I2C4_DEVFN		PCI_DEVFN(0x17, 0)
#define I2C5_DEVFN		PCI_DEVFN(0x17, 1)
#define I2C6_DEVFN		PCI_DEVFN(0x17, 2)
#define I2C7_DEVFN		PCI_DEVFN(0x17, 3)
#define UART0_DEVFN		PCI_DEVFN(0x18, 0)
#define UART1_DEVFN		PCI_DEVFN(0x18, 1)
#define UART2_DEVFN		PCI_DEVFN(0x18, 2)
#define UART3_DEVFN		PCI_DEVFN(0x18, 3)
#define SPI0_DEVFN		PCI_DEVFN(0x19, 0)
#define SPI1_DEVFN		PCI_DEVFN(0x19, 1)
#define SPI2_DEVFN		PCI_DEVFN(0x19, 2)
#define SDCARD_DEVFN		PCI_DEVFN(0x1b, 0)
#define EMMC_DEVFN		PCI_DEVFN(0x1c, 0)
#define SDIO_DEVFN		PCI_DEVFN(0x1e, 0)
#define SMBUS_DEVFN		PCI_DEVFN(0x1f, 1)
#endif
