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
#include "it8728f.h"

/* The base address is 0x2e or 0x4e, depending on config bytes. */
#define SIO_BASE                     0x2e
#define SIO_INDEX                    SIO_BASE
#define SIO_DATA                     (SIO_BASE + 1)

/* Global configuration registers. */
#define IT8728F_CONFIG_REG_CC        0x02 /* Configure Control (write-only). */
#define IT8728F_CONFIG_REG_LDN       0x07 /* Logical Device Number. */
#define IT8728F_CONFIG_REG_CHIPVERS  0x22 /* Chip version */
#define IT8728F_CONFIG_REG_CLOCKSEL  0x23 /* Clock Selection. */
#define IT8728F_CONFIG_REG_SWSUSP    0x24 /* Software Suspend, Flash I/F. 'Special register' */

static void it8728f_sio_write(u8 ldn, u8 index, u8 value)
{
	outb(IT8728F_CONFIG_REG_LDN, SIO_BASE);
	outb(ldn, SIO_DATA);
	outb(index, SIO_BASE);
	outb(value, SIO_DATA);
}

static void it8728f_enter_conf(device_t dev)
{
	u16 port = dev >> 8;

	outb(0x87, port);
	outb(0x01, port);
	outb(0x55, port);
	outb((port == 0x4e) ? 0xaa : 0x55, port);
}

static void it8728f_exit_conf(void)
{
	it8728f_sio_write(0x00, IT8728F_CONFIG_REG_CC, 0x02);
}

/* Select 24MHz CLKIN (48MHz default). */
void it8728f_24mhz_clkin(device_t dev)
{
	it8728f_enter_conf(dev);
	it8728f_sio_write(0x00, IT8728F_CONFIG_REG_CLOCKSEL, 0x1);
	it8728f_exit_conf();
}

/*
 * GIGABYTE uses a special Super I/O register to protect its Dual BIOS
 * mechanism. It lives in the GPIO LDN. However, register 0xEF is not
 * mentioned in the IT8728F datasheet so just hardcode it to 0x7E for now.
 * 
 * Bit 0 is dualbios SPI chipselect, the rest set to 0x7E makes it boot
 * without falling into power cycle loop.
 */
void it8728f_disable_reboot(device_t dev)
{
	it8728f_enter_conf(dev);
	it8728f_sio_write(IT8728F_GPIO, 0xEF, 0x7E);
	it8728f_exit_conf();
}

/* Enable the serial port(s). */
void it8728f_enable_serial(device_t dev, u16 iobase)
{
	/* (1) Enter the configuration state (MB PnP mode). */
	it8728f_enter_conf(dev);

	/* (2) Modify the data of configuration registers. */
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);

	/* (3) Exit the configuration state (MB PnP mode). */
	it8728f_exit_conf();
}
