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

#define _LPSS_PCI_DEVFN(slot, func)	PCI_DEVFN(LPSS_DEV_SLOT_##slot, func)
#define _PCI_DEVFN(slot, func)		PCI_DEVFN(slot, func)

#if !defined(__SIMPLE_DEVICE__)
#include <device/device.h>
#include <device/pci_def.h>
#define _LPSS_PCI_DEV(slot, func)	dev_find_slot(0, _LPSS_PCI_DEVFN(slot, func))
#define _PCI_DEV(slot, func)		dev_find_slot(0, PCI_DEVFN(slot, func))
#else
#include <arch/io.h>
#define _LPSS_PCI_DEV(slot, func)	PCI_DEV(0, LPSS_DEV_SLOT_##slot, func)
#define _PCI_DEV(slot, func)		PCI_DEV(0, slot, func)
#endif

#define NB_DEVFN			_PCI_DEVFN(0, 0)
#define NB_DEV_ROOT			_PCI_DEV(0x0, 0)

#define PUNIT_DEV			_PCI_DEV(0, 1)
#define PUNIT_DEVFN			_PCI_DEVFN(0x0, 1)

#define IGD_DEV				_PCI_DEV(0x2, 0)
#define IGD_DEVFN			_PCI_DEVFN(0x2, 0)

#define P2SB_DEV			_PCI_DEV(0xd, 0)
#define P2SB_DEVFN			_PCI_DEVFN(0xd, 0)

#define PMC_DEV				_PCI_DEV(0xd, 1)
#define PMC_DEVFN			_PCI_DEVFN(0xd, 1)

#define SPI_DEV				_PCI_DEV(0xd, 2)
#define SPI_DEVFN			_PCI_DEVFN(0xd, 2)

#define HDA_DEV				_PCI_DEV(0xe, 0)
#define HDA_DEVFN			_PCI_DEVFN(0xe, 0)

#define HECI1_DEV			_PCI_DEV(0xf, 0)
#define HECI1_DEVFN			_PCI_DEVFN(0xf, 0)

#define ISH_DEV				_PCI_DEV(0x11, 0)
#define ISH_DEVFN			_PCI_DEVFN(0x11, 0)

#define SATA_DEV			_PCI_DEV(0x12, 0)
#define SATA_DEVFN			_PCI_DEVFN(0x12, 0)

#define PCIEA0_DEVFN			_PCI_DEVFN(0x13, 0)
#define PCIEA1_DEVFN			_PCI_DEVFN(0x13, 1)
#define PCIEA2_DEVFN			_PCI_DEVFN(0x13, 2)
#define PCIEA3_DEVFN			_PCI_DEVFN(0x13, 3)
#define PCIEB0_DEVFN			_PCI_DEVFN(0x14, 0)
#define PCIEB1_DEVFN			_PCI_DEVFN(0x14, 1)

#define XHCI_DEV			_PCI_DEV(0x15, 0)
#define XHCI_DEVFN			_PCI_DEVFN(0x15, 0)

#define XDCI_DEV			_PCI_DEV(0x15, 1)
#define XDCI_DEVFN			_PCI_DEVFN(0x15, 1)

/* LPSS I2C, 2 devices cover 8 controllers */
#define LPSS_DEV_SLOT_I2C_D0		0x16
#define  LPSS_DEVFN_I2C0		_LPSS_PCI_DEVFN(I2C_D0, 0)
#define  LPSS_DEVFN_I2C1		_LPSS_PCI_DEVFN(I2C_D0, 1)
#define  LPSS_DEVFN_I2C2		_LPSS_PCI_DEVFN(I2C_D0, 2)
#define  LPSS_DEVFN_I2C3		_LPSS_PCI_DEVFN(I2C_D0, 3)
#define LPSS_DEV_SLOT_I2C_D1		0x17
#define  LPSS_DEVFN_I2C4		_LPSS_PCI_DEVFN(I2C_D1, 0)
#define  LPSS_DEVFN_I2C5		_LPSS_PCI_DEVFN(I2C_D1, 1)
#define  LPSS_DEVFN_I2C6		_LPSS_PCI_DEVFN(I2C_D1, 2)
#define  LPSS_DEVFN_I2C7		_LPSS_PCI_DEVFN(I2C_D1, 3)

/* LPSS UART */
#define LPSS_DEV_SLOT_UART		0x18
#define  LPSS_DEVFN_UART0		_LPSS_PCI_DEVFN(UART, 0)
#define  LPSS_DEVFN_UART1		_LPSS_PCI_DEVFN(UART, 1)
#define  LPSS_DEVFN_UART2		_LPSS_PCI_DEVFN(UART, 2)
#define  LPSS_DEVFN_UART3		_LPSS_PCI_DEVFN(UART, 3)
#define  LPSS_DEV_UART0			_LPSS_PCI_DEV(UART, 0)
#define  LPSS_DEV_UART1			_LPSS_PCI_DEV(UART, 1)
#define  LPSS_DEV_UART2			_LPSS_PCI_DEV(UART, 2)
#define  LPSS_DEV_UART3			_LPSS_PCI_DEV(UART, 3)

/* LPSS SPI */
#define LPSS_DEV_SLOT_SPI		0x19
#define  LPSS_DEVFN_SPI0		_LPSS_PCI_DEVFN(SPI, 0)
#define  LPSS_DEVFN_SPI1		_LPSS_PCI_DEVFN(SPI, 1)
#define  LPSS_DEVFN_SPI2		_LPSS_PCI_DEVFN(SPI, 2)

/* LPSS PWM */
#define LPSS_DEV_SLOT_PWM		0x1a
#define  LPSS_DEVFN_PWM			_LPSS_PCI_DEVFN(PWM, 0)

#define SDCARD_DEV			_PCI_DEV(0x1b, 0)
#define SDCARD_DEVFN			_PCI_DEVFN(0x1b, 0)

#define EMMC_DEV			_PCI_DEV(0x1c, 0)
#define EMMC_DEVFN			_PCI_DEVFN(0x1c, 0)

#define SDIO_DEV			_PCI_DEV(0x1e, 0)
#define SDIO_DEVFN			_PCI_DEVFN(0x1e, 0)

#define LPC_DEV				_PCI_DEV(0x1f, 0)
#define LPC_DEVFN			_PCI_DEVFN(0x1f, 0)

#define SMBUS_DEV			_PCI_DEV(0x1f, 1)
#define SMBUS_DEVFN			_PCI_DEVFN(0x1f, 1)

#endif
