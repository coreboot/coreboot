/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 University of Heidelberg
 * Written by Mondrian Nuessle <nuessle@uni-heidelberg.de> for Univ. Heidelberg
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

/* PILOT Super I/O is only based on LPC observation done on factory system. */

#define BLUBB_DEV PNP_DEV(port, 0x04)

/*
 * Logical device 4, 5 and 7 are being deactivated. Logical Device 1 seems to
 * be another serial (?), it is also deactivated on the HP machine.
 */
static void pilot_early_init(device_t dev)
{
	u16 port = dev >> 8;

	print_debug("Using port: ");
	print_debug_hex16(port);
	print_debug("\n");
	pilot_disable_serial(PNP_DEV(port, 0x1));
	print_debug("disable serial 1\n");

	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(PNP_DEV(port, 0x3));
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, 0x60, 0x0b00);
	pnp_set_iobase(dev, 0x62, 0x0b80);
	pnp_set_iobase(dev, 0x64, 0x0b84);
	pnp_set_iobase(dev, 0x66, 0x0b86);
	pnp_set_enable(dev, 1);
	pnp_exit_ext_func_mode(dev);

/*
	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(PNP_DEV(port, 0x3));
	pnp_exit_ext_func_mode(dev);
	pnp_enter_ext_func_mode(dev);
	pnp_set_enable(PNP_DEV(port, 0x3), 0);
	pnp_exit_ext_func_mode(dev);
*/

	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(PNP_DEV(port, 0x4));
	pnp_exit_ext_func_mode(dev);
	pnp_enter_ext_func_mode(dev);
	pnp_set_enable( PNP_DEV(port, 0x4), 0);
	pnp_exit_ext_func_mode(dev);

	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(PNP_DEV(port, 0x5));
	pnp_exit_ext_func_mode(dev);
	pnp_enter_ext_func_mode(dev);
	pnp_set_enable(PNP_DEV(port, 0x5), 0);
	pnp_exit_ext_func_mode(dev);

	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(PNP_DEV(port, 0x6));
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x60);
	pnp_set_iobase(dev, PNP_IDX_IO1, 0x64);
	pnp_set_irq(dev, PNP_IDX_IRQ0, 1);
	pnp_set_drq(dev, 0x71, 3);
	pnp_set_enable(dev, 0);
	pnp_exit_ext_func_mode(dev);

	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(PNP_DEV(port, 0xe));
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x70);
	pnp_set_iobase(dev, PNP_IDX_IO1, 0x72);
	pnp_set_irq(dev, PNP_IDX_IRQ0, 8);
	pnp_set_drq(dev, 0x71, 3);
	pnp_set_enable(dev, 0);
	pnp_exit_ext_func_mode(dev);

	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(PNP_DEV(port, 0x7));
	pnp_exit_ext_func_mode(dev);
	pnp_enter_ext_func_mode(dev);
	pnp_set_enable(PNP_DEV(port, 0x7), 0);
	pnp_exit_ext_func_mode(dev);

/*
	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(PNP_DEV(port, 0x8));
	pnp_exit_ext_func_mode(dev);
	pnp_enter_ext_func_mode(dev);
	pnp_set_enable(PNP_DEV(port, 0x8), 0);
	pnp_exit_ext_func_mode(dev);

	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(PNP_DEV(port, 0x9));
	pnp_exit_ext_func_mode(dev);
	pnp_enter_ext_func_mode(dev);
	pnp_set_enable(PNP_DEV(port, 0x9), 0);
	pnp_exit_ext_func_mode(dev);

	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(PNP_DEV(port, 0x10));
	pnp_exit_ext_func_mode(dev);
	pnp_enter_ext_func_mode(dev);
	pnp_set_enable(PNP_DEV(port, 0x10), 0);
	pnp_exit_ext_func_mode(dev);
*/
}
