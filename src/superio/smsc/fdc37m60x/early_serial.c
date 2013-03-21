/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include "fdc37m60x.h"

/* The base address is 0x3f0 or 0x370, depending on the SYSOPT pin. */
#define SIO_BASE                       0x3f0
#define SIO_INDEX                      SIO_BASE
#define SIO_DATA                       (SIO_BASE + 1)

/* Global configuration registers. */
#define FDC37M60X_CONFIG_REG_CC        0x02  /* Configure Control. */
#define FDC37M60X_CONFIG_REG_LDN       0x07  /* Logical Device Number. */
#define FDC37M60X_CONFIG_POWER_CONTROL 0x22  /* Power Control. */
#define FDC37M60X_CONFIG_POWER_MGMT    0x23  /* Intelligent Power Mgmt. */
#define FDC37M60X_CONFIG_OSC           0x24  /* OSC. */

#define FDC37M60X_CONFIGURATION_PORT   0x3f0 /* Write-only. */

/* The content of FDC37M60X_CONFIG_REG_LDN (index 0x07) must be set to the
   LDN the register belongs to, before you can access the register. */
static void fdc37m60x_sio_write(uint8_t ldn, u8 index, u8 value)
{
	outb(FDC37M60X_CONFIG_REG_LDN, SIO_BASE);
	outb(ldn, SIO_DATA);
	outb(index, SIO_BASE);
	outb(value, SIO_DATA);
}

/* Enable the peripheral devices on the FDC37M60X Super I/O chip. */
static void fdc37m60x_enable_serial(device_t dev, u16 iobase)
{
	/* (1) Enter the configuration state. */
	outb(0x55, FDC37M60X_CONFIGURATION_PORT);

	/* (2) Modify the data of configuration registers. */

	/* Power on all devices by setting the respective bit.
	   Bits: 0 (FDC), 3 (PP), 4 (Com1), 5 (Com2). The rest is reserved. */
	fdc37m60x_sio_write(0x00, FDC37M60X_CONFIG_POWER_CONTROL, 0x39);

	/* Disable intelligent power management. */
	fdc37m60x_sio_write(0x00, FDC37M60X_CONFIG_POWER_MGMT, 0x00);

	/* Turn on OSC, turn on BRG clock. */
	fdc37m60x_sio_write(0x00, FDC37M60X_CONFIG_OSC, 0x04);

	/* Configure serial port 1. */
	fdc37m60x_sio_write(FDC37M60X_SP1, 0x60, 0x03);
	fdc37m60x_sio_write(FDC37M60X_SP1, 0x61, 0xf8); /* I/O 0x3f8 */
	fdc37m60x_sio_write(FDC37M60X_SP1, 0x70, 0x04); /* IRQ 4 */
	fdc37m60x_sio_write(FDC37M60X_SP1, 0xf0, 0x00); /* Normal */

	/* Enable serial port 1. */
	fdc37m60x_sio_write(FDC37M60X_SP1, 0x30, 0x01);

	/* (3) Exit the configuration state. */
	outb(0xaa, FDC37M60X_CONFIGURATION_PORT);
}
