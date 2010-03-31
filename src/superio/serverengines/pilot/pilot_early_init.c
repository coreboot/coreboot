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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/* PILOT Super I/O is only based on LPC observation done on factory system. */

#define BLUBB_DEV PNP_DEV(port, 0x04)

/*
 * Logical device 4, 5 and 7 are being deactivated. Logical Device 1 seems to
 * be another serial (?), it is also deactivated on the HP machine.
 */
static void pilot_early_init(device_t dev)
{
	unsigned port = dev >> 8;

	print_debug("Using port: ");
	print_debug_hex16(port);
	print_debug("\n");
	pilot_disable_serial(PNP_DEV(port, 0x1));
	print_debug("disable serial 1\n");

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
/*
	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(PNP_DEV(port, 0x6));
	pnp_exit_ext_func_mode(dev);
	pnp_enter_ext_func_mode(dev);
	pnp_set_enable( PNP_DEV(port, 0x6), 0);
	pnp_exit_ext_func_mode(dev);
*/
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
