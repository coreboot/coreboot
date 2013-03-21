/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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


#include <arch/io.h>
#include "sch4037.h"

static inline void pnp_enter_conf_state(device_t dev)
{
	unsigned port = dev>>8;
	outb(0x55, port);
}

static void pnp_exit_conf_state(device_t dev)
{
	unsigned port = dev>>8;
	outb(0xaa, port);
}

static inline void sch4037_early_init(unsigned port)
{
	device_t dev;

	dev = PNP_DEV(port, SMSCSUPERIO_SP1);
	pnp_enter_conf_state(dev);

	/* Auto power management */
	pnp_write_config(dev, 0x22, 0x38); /* BIT3+BIT4+BIT5 */
	pnp_write_config(dev, 0x23, 0 );

	/* Enable SMSC UART 0 */
	dev = PNP_DEV(port, SMSCSUPERIO_SP1);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);

	pnp_set_iobase(dev, PNP_IDX_IO0, CONFIG_TTYS0_BASE);
	pnp_set_irq(dev, PNP_IDX_IRQ0, 0x4);

	/* Enabled High speed, disabled MIDI support. */
	pnp_write_config(dev, 0xF0, 0x02);
	pnp_set_enable(dev, 1);

	/* Enable keyboard */
	dev = PNP_DEV(port, SCH4037_KBC);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_irq(dev, 0x70, 1);   /* IRQ 1 */
	pnp_set_irq(dev, 0x72, 12);   /* IRQ 12 */
	pnp_set_enable(dev, 1);

	pnp_exit_conf_state(dev);
}

