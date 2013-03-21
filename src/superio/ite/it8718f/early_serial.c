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
#include "it8718f.h"

/* The base address is 0x2e or 0x4e, depending on config bytes. */
#define SIO_BASE                     0x2e
#define SIO_INDEX                    SIO_BASE
#define SIO_DATA                     (SIO_BASE + 1)

/* Global configuration registers. */
#define IT8718F_CONFIG_REG_CC        0x02 /* Configure Control (write-only). */
#define IT8718F_CONFIG_REG_LDN       0x07 /* Logical Device Number. */
#define IT8718F_CONFIG_REG_CONFIGSEL 0x22 /* Configuration Select. */
#define IT8718F_CONFIG_REG_CLOCKSEL  0x23 /* Clock Selection. */
#define IT8718F_CONFIG_REG_SWSUSP    0x24 /* Software Suspend, Flash I/F. */

static void it8718f_sio_write(u8 ldn, u8 index, u8 value)
{
	outb(IT8718F_CONFIG_REG_LDN, SIO_BASE);
	outb(ldn, SIO_DATA);
	outb(index, SIO_BASE);
	outb(value, SIO_DATA);
}

static void it8718f_enter_conf(void)
{
	u16 port = 0x2e; /* TODO: Don't hardcode! */

	outb(0x87, port);
	outb(0x01, port);
	outb(0x55, port);
	outb((port == 0x4e) ? 0xaa : 0x55, port);
}

static void it8718f_exit_conf(void)
{
	it8718f_sio_write(0x00, IT8718F_CONFIG_REG_CC, 0x02);
}

/* Select 24MHz CLKIN (48MHz default). */
void it8718f_24mhz_clkin(void)
{
	it8718f_enter_conf();
	it8718f_sio_write(0x00, IT8718F_CONFIG_REG_CLOCKSEL, 0x1);
	it8718f_exit_conf();
}

/*
 * GIGABYTE uses a special Super I/O register to protect its Dual BIOS
 * mechanism. It lives in the GPIO LDN. However, register 0xEF is not
 * mentioned in the IT8718F datasheet so just hardcode it to 0x7E for now.
 */
void it8718f_disable_reboot(void)
{
	it8718f_enter_conf();
	it8718f_sio_write(IT8718F_GPIO, 0xEF, 0x7E);
	it8718f_exit_conf();
}

/* Enable the serial port(s). */
void it8718f_enable_serial(device_t dev, u16 iobase)
{
	/* (1) Enter the configuration state (MB PnP mode). */
	it8718f_enter_conf();

	/* (2) Modify the data of configuration registers. */

	/*
	 * Select the chip to configure (if there's more than one).
	 * Set bit 7 to select JP3=1, clear bit 7 to select JP3=0.
	 * If this register is not written, both chips are configured.
	 */

	/* it8718f_sio_write(0x00, IT8718F_CONFIG_REG_CONFIGSEL, 0x00); */

	/* Enable serial port(s). */
	it8718f_sio_write(IT8718F_SP1, 0x30, 0x1); /* Serial port 1 */
	it8718f_sio_write(IT8718F_SP2, 0x30, 0x1); /* Serial port 2 */

	/* Clear software suspend mode (clear bit 0). TODO: Needed? */
	/* it8718f_sio_write(0x00, IT8718F_CONFIG_REG_SWSUSP, 0x00); */

	/* (3) Exit the configuration state (MB PnP mode). */
	it8718f_exit_conf();
}
